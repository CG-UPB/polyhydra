
#include "Window.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "input/Input.h"
#include "util/UIUtil.h"
#include "fs/FileManager.h"
#include "rendering/gl/Shader.h"
#include "rendering/gl/VertexArrayObject.h"

#include <stb_image.h>

namespace volumeshOS::Internal
{
    static void glfw_error_callback(int error, const char* description)
    {
        fprintf(stderr, "Glfw Error %d: %s\n", error, description);
    }

    static void print_error(const std::string& description)
    {
        fprintf(stderr, "Error: %s\n", description.c_str());
    }

    Window::Window(int width, int height, std::string title):
        m_width(width),
        m_height(height),
        m_title(std::move(title)),
        m_window(nullptr)
    {}

    void Window::initialize()
    {
        // initialize libraries
        init_glfw();
        init_imgui();
        init_style();
        Shader::load_all();
        UIUtil::load_all();
        VertexArrayObject::init();

        // create ui panels
        panels.mesh_layer_view      = std::make_shared<MeshLayerView>();
        panels.mesh_view            = std::make_shared<MeshView>(m_width, m_height);
        panels.quality_settings     = std::make_shared<QualityPanel>();
        panels.toolbar              = std::make_shared<ToolBar>();

        m_open = true;
    }

    void Window::clean_up()
    {
        // clean up ui panels
        panels.mesh_layer_view      = nullptr;
        panels.mesh_view            = nullptr;
        panels.quality_settings     = nullptr;
        panels.toolbar              = nullptr;

        // clean up
        VertexArrayObject::clean_up();
        UIUtil::clean_up();
        Shader::delete_all();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(get_window());
        glfwTerminate();

        m_open = false;
    }


    void Window::init_glfw()
    {
        // Setup window
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit())
        {
            return;
        }

        // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
        // GL ES 2.0 + GLSL 100
        m_glslVersion = "#version 100";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
        // GL 3.2 + GLSL 150
        m_glslVersion = "#version 150";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
        // GL 3.0 + GLSL 130
        m_glslVersion = "#version 330 core";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
        glfwWindowHint(GLFW_DEPTH_BITS, 24);
        glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

        // Create window with graphics context
        m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
        if (m_window == nullptr)
        {
            print_error("Failed to create window");
            return;
        }

        GLFWimage images[1];
        images[0].pixels = stbi_load("./res/icons/logo.png", &images[0].width, &images[0].height, 0, 4);
        glfwSetWindowIcon(m_window, 1, images);
        stbi_image_free(images[0].pixels);

        glfwMakeContextCurrent(m_window);
        glfwSwapInterval(1); // Enable vsync

        Input::setup(m_window);

        // update opengl functions
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            print_error("Failed to initialize OpenGL context");
            return;
        }

        glEnable(GL_MULTISAMPLE);
    }

    void Window::init_imgui()
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        // we have to create a new string here, otherwise it would be deallocated from the stack before imgui uses it
        FS_NAMESPACE::path iniPath = FileManager::get_resource_path() / "config.ini";
        io.IniFilename = (new std::string(iniPath.string()))->c_str();

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();


        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(m_window, true);
        ImGui_ImplOpenGL3_Init(m_glslVersion.c_str());
    }

    void Window::init_style()
    {
        ImGuiStyle& style = ImGui::GetStyle();

        // Rounding
        style.FrameRounding = 4.0f;
        style.GrabRounding = 4.0f;
        style.WindowRounding = 6.0f;
        style.PopupRounding = 6.0f;
        style.WindowPadding = {20.0f, 20.0f};
        style.FramePadding = {10.0f, 8.0f};
        style.ItemSpacing = {14.0f, 0.0f};
        style.ItemInnerSpacing = {2.0f, 0.0f};
        style.TabRounding = 4.0f;
        style.FrameBorderSize = 1.0f;
        style.TabBorderSize = 1.0f;
        style.GrabMinSize = 20.0f;
        style.ChildBorderSize = 0.0f;
        style.CellPadding = {0.0f, 1.0f};
        style.ChildBorderSize = 1.0f;
        style.ChildRounding = 4.0f;
        style.GrabRounding = 3.0f;
        style.WindowMenuButtonPosition = ImGuiDir_None;

        load_light_mode();
    }

    void Window::show_dock_space()
    {
        static bool dockSpaceOpen = true;
        static ImGuiDockNodeFlags dockSpaceFlags = ImGuiDockNodeFlags_None;

        ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        ImGuiWindowFlags window_flags =
                        ImGuiWindowFlags_NoDocking |
                        ImGuiWindowFlags_NoTitleBar |
                        ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoMove |
                        ImGuiWindowFlags_NoBringToFrontOnFocus |
                        ImGuiWindowFlags_NoNavFocus;

        if (dockSpaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &dockSpaceOpen, window_flags);
        ImGui::PopStyleVar(3);

        // DockSpace
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        float minWinSizeX = style.WindowMinSize.x;
        style.WindowMinSize.x = 370.0f;
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockSpaceID = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockSpaceID, ImVec2(0.0f, 0.0f), dockSpaceFlags);
        }
        style.WindowMinSize.x = minWinSizeX;
    }

    bool Window::should_close()
    {
        return glfwWindowShouldClose(get_window()) || !m_open;
    }

    void Window::close()
    {
        m_open = false;
    }

    void Window::pre_render_step()
    {

        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        show_dock_space();

        //ImGui::ShowDemoWindow();
    }

    void Window::render()
    {
        pre_render_step();

        panels.mesh_layer_view->show();
        panels.mesh_view->show();
        //panels.quality_settings->show();
        panels.toolbar->show(panels.mesh_view->renderer->camera);

        AppState::callbacks.on_gui_render();

        post_render_step();
    }

    void Window::post_render_step()
    {

        ImGui::End();

        // Rendering
        ImGui::Render();
        int displayWidth, displayHeight;
        glfwGetFramebufferSize(get_window(), &displayWidth, &displayHeight);

        glViewport(0, 0, displayWidth, displayHeight);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(get_window());

        Input::reset_offset();
    }

    void Window::load_light_mode()
    {
        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text]                   = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
        colors[ImGuiCol_TextDisabled]           = ImVec4(0.33f, 0.44f, 0.53f, 1.00f);
        colors[ImGuiCol_WindowBg]               = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_ChildBg]                = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);
        colors[ImGuiCol_PopupBg]                = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_Border]                 = ImVec4(0.74f, 0.74f, 0.74f, 0.49f);
        colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg]                = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
        colors[ImGuiCol_TitleBg]                = ImVec4(0.49f, 0.56f, 0.70f, 1.00f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.51f, 0.58f, 0.71f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.49f, 0.56f, 0.70f, 1.00f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.77f, 0.77f, 0.77f, 1.00f);
        colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.64f, 0.64f, 0.64f, 0.00f);
        colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.83f, 0.83f, 0.83f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.77f, 0.77f, 0.77f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.68f, 0.68f, 0.68f, 1.00f);
        colors[ImGuiCol_CheckMark]              = ImVec4(0.40f, 0.48f, 0.61f, 1.00f);
        colors[ImGuiCol_SliderGrab]             = ImVec4(0.50f, 0.59f, 0.70f, 1.00f);
        colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.52f, 0.61f, 0.72f, 1.00f);
        colors[ImGuiCol_Button]                 = ImVec4(0.65f, 0.78f, 0.93f, 1.00f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.68f, 0.80f, 0.96f, 1.00f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.69f, 0.82f, 0.97f, 1.00f);
        colors[ImGuiCol_Header]                 = ImVec4(0.72f, 0.77f, 0.88f, 1.00f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.74f, 0.79f, 0.90f, 1.00f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.76f, 0.81f, 0.92f, 1.00f);
        colors[ImGuiCol_Separator]              = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
        colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.73f, 0.73f, 0.73f, 1.00f);
        colors[ImGuiCol_SeparatorActive]        = ImVec4(0.66f, 0.66f, 0.66f, 1.00f);
        colors[ImGuiCol_ResizeGrip]             = ImVec4(0.58f, 0.58f, 0.58f, 0.25f);
        colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.39f, 0.39f, 0.39f, 0.67f);
        colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.65f, 0.65f, 0.65f, 0.95f);
        colors[ImGuiCol_Tab]                    = ImVec4(0.93f, 0.93f, 0.94f, 1.00f);
        colors[ImGuiCol_TabHovered]             = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
        colors[ImGuiCol_TabActive]              = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);
        colors[ImGuiCol_TabUnfocused]           = ImVec4(0.87f, 0.87f, 0.87f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);
        colors[ImGuiCol_DockingPreview]         = ImVec4(0.65f, 0.65f, 0.65f, 0.70f);
        colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
        colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
        colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
        colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    }

    void Window::load_dark_mode()
    {
        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text]                   = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
        colors[ImGuiCol_TextDisabled]           = ImVec4(0.33f, 0.44f, 0.53f, 1.00f);
        colors[ImGuiCol_WindowBg]               = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
        colors[ImGuiCol_ChildBg]                = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
        colors[ImGuiCol_PopupBg]                = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_Border]                 = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
        colors[ImGuiCol_BorderShadow]           = ImVec4(0.22f, 0.22f, 0.22f, 0.00f);
        colors[ImGuiCol_FrameBg]                = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
        colors[ImGuiCol_TitleBg]                = ImVec4(0.23f, 0.26f, 0.29f, 1.00f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.25f, 0.27f, 0.30f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.23f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
        colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.21f, 0.21f, 0.21f, 0.39f);
        colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
        colors[ImGuiCol_CheckMark]              = ImVec4(0.39f, 0.48f, 0.62f, 1.00f);
        colors[ImGuiCol_SliderGrab]             = ImVec4(0.23f, 0.38f, 0.56f, 1.00f);
        colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.26f, 0.41f, 0.59f, 1.00f);
        colors[ImGuiCol_Button]                 = ImVec4(0.20f, 0.26f, 0.34f, 1.00f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.28f, 0.41f, 0.57f, 1.00f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.35f, 0.46f, 0.68f, 1.00f);
        colors[ImGuiCol_Header]                 = ImVec4(0.23f, 0.25f, 0.28f, 1.00f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.35f, 0.44f, 0.64f, 1.00f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.37f, 0.47f, 0.66f, 1.00f);
        colors[ImGuiCol_Separator]              = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
        colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.39f, 0.39f, 0.39f, 0.78f);
        colors[ImGuiCol_SeparatorActive]        = ImVec4(0.66f, 0.66f, 0.66f, 1.00f);
        colors[ImGuiCol_ResizeGrip]             = ImVec4(0.18f, 0.18f, 0.18f, 0.25f);
        colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.39f, 0.39f, 0.39f, 0.67f);
        colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.65f, 0.65f, 0.65f, 0.95f);
        colors[ImGuiCol_Tab]                    = ImVec4(0.16f, 0.16f, 0.17f, 1.00f);
        colors[ImGuiCol_TabHovered]             = ImVec4(0.26f, 0.26f, 0.26f, 0.80f);
        colors[ImGuiCol_TabActive]              = ImVec4(0.31f, 0.35f, 0.39f, 1.00f);
        colors[ImGuiCol_TabUnfocused]           = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.31f, 0.35f, 0.39f, 1.00f);
        colors[ImGuiCol_DockingPreview]         = ImVec4(0.65f, 0.65f, 0.65f, 0.70f);
        colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
        colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
        colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
        colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    }

}
