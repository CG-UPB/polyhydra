
#include <glad/glad.h>

#include <utility>
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "ImguiRenderer.h"
#include "input/Input.h"
#include "fs/FileManager.h"
#include "Window.h"
#include "panels/LogWindow.h"
#include "rendering/gl/Shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace vOS
{
    static void glfwErrorCallback(int error, const char *description)
    {
        fprintf(stderr, "Glfw Error %d: %s\n", error, description);
    }

    static void printError(const std::string& description)
    {
        fprintf(stderr, "Error: %s\n", description.c_str());
    }


    ImguiRenderer::ImguiRenderer(int width, int height, std::string title): m_width(width), m_height(height), m_title(std::move(title))
    {
        initGLFW();
        initImGui();
        initImGuiStyle();
        Shader::load_all();
    }

    ImguiRenderer::~ImguiRenderer()
    {
        Shader::delete_all();
        VertexArrayObject::clean_up();

        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(get_window());

        glfwTerminate();
    }


    void ImguiRenderer::initGLFW()
    {
        // Setup window
        glfwSetErrorCallback(glfwErrorCallback);
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
            printError("Failed to create window");
            return;
        }

        GLFWimage images[1];
        images[0].pixels = stbi_load("../res/logo.png", &images[0].width, &images[0].height, 0, 4);
        glfwSetWindowIcon(m_window, 1, images);
        stbi_image_free(images[0].pixels);

        glfwMakeContextCurrent(m_window);
        glfwSwapInterval(1); // Enable vsync
        // Setup Input Class
        Input::setup(m_window);
        /*
        glfwSetKeyCallback(m_window, Input::glfwKeyCallback);
        glfwSetMouseButtonCallback(m_window, Input::glfwMouseButtonCallback);
        glfwSetCursorPosCallback(m_window, Input::glfwMouseCursorPosCallback);
        glfwSetScrollCallback(m_window, Input::glfwScrollCallback);
         */

        // load opengl functions
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            printError("Failed to initialize OpenGL context");
            return;
        }

        glEnable(GL_MULTISAMPLE);
    }

    void ImguiRenderer::initImGui()
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

    void ImguiRenderer::initImGuiStyle()
    {
        // Font
        FS_NAMESPACE::path fontPath = FileManager::get_resource_path() / "fonts" / "Roboto-Regular.ttf";
        ImGui::GetIO().Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 18.0f);
        ImGuiStyle& style = ImGui::GetStyle();

        // Rounding
        style.FrameRounding = 4.0f;
        style.GrabRounding = 4.0f;
        style.WindowRounding = 6.0f;
        style.PopupRounding = 6.0f;
        style.WindowPadding = {20.0f, 20.0f};
        style.FramePadding = {10.0f, 4.0f};
        style.ItemSpacing = {12.0f, 6.0f};
        style.TabRounding = 4.0f;
        style.FrameBorderSize = 1.0f;
        style.TabBorderSize = 1.0f;
        style.GrabMinSize = 20.0f;
        style.ChildBorderSize = 0.0f;

        load_light_mode();
    }

    void ImguiRenderer::show_dock_space()
    {
        // Note: Switch this to true to enable dockspace
        static bool dockSpaceOpen = true;
        static ImGuiDockNodeFlags dockSpaceFlags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockSpaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
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

    bool ImguiRenderer::window_closed() {
        return glfwWindowShouldClose(get_window());
    }

    void ImguiRenderer::pre_render_step() {

        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        show_dock_space();

        //ImGui::ShowDemoWindow();
    }

    void ImguiRenderer::post_render_step() {

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

    void ImguiRenderer::load_light_mode()
    {
        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text]                   = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
        colors[ImGuiCol_TextDisabled]           = ImVec4(0.33f, 0.44f, 0.53f, 1.00f);
        colors[ImGuiCol_WindowBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.89f);
        colors[ImGuiCol_ChildBg]                = ImVec4(0.89f, 0.89f, 0.89f, 0.65f);
        colors[ImGuiCol_PopupBg]                = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_Border]                 = ImVec4(0.74f, 0.74f, 0.74f, 0.49f);
        colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg]                = ImVec4(1.00f, 1.00f, 1.00f, 0.66f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.95f, 0.95f, 0.95f, 0.68f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
        colors[ImGuiCol_TitleBg]                = ImVec4(0.78f, 0.78f, 0.78f, 0.90f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.81f, 0.81f, 0.81f, 0.83f);
        colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.78f, 0.78f, 0.78f, 0.51f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.77f, 0.77f, 0.77f, 1.00f);
        colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.64f, 0.64f, 0.64f, 0.00f);
        colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.68f, 0.68f, 0.68f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
        colors[ImGuiCol_CheckMark]              = ImVec4(0.53f, 0.53f, 0.53f, 1.00f);
        colors[ImGuiCol_SliderGrab]             = ImVec4(0.67f, 0.67f, 0.67f, 1.00f);
        colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.63f, 0.63f, 0.63f, 1.00f);
        colors[ImGuiCol_Button]                 = ImVec4(0.79f, 0.79f, 0.79f, 0.74f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.72f, 0.72f, 0.72f, 1.00f);
        colors[ImGuiCol_Header]                 = ImVec4(0.73f, 0.73f, 0.73f, 0.75f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.71f, 0.71f, 0.71f, 0.80f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
        colors[ImGuiCol_Separator]              = ImVec4(0.80f, 0.80f, 0.80f, 0.92f);
        colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.39f, 0.39f, 0.39f, 0.78f);
        colors[ImGuiCol_SeparatorActive]        = ImVec4(0.66f, 0.66f, 0.66f, 1.00f);
        colors[ImGuiCol_ResizeGrip]             = ImVec4(0.18f, 0.18f, 0.18f, 0.25f);
        colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.39f, 0.39f, 0.39f, 0.67f);
        colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.65f, 0.65f, 0.65f, 0.95f);
        colors[ImGuiCol_Tab]                    = ImVec4(0.75f, 0.75f, 0.75f, 0.51f);
        colors[ImGuiCol_TabHovered]             = ImVec4(0.65f, 0.65f, 0.65f, 0.80f);
        colors[ImGuiCol_TabActive]              = ImVec4(1.00f, 1.00f, 1.00f, 0.82f);
        colors[ImGuiCol_TabUnfocused]           = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(1.00f, 1.00f, 1.00f, 0.82f);
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

    void ImguiRenderer::load_dark_mode()
    {
        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text]                   = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
        colors[ImGuiCol_TextDisabled]           = ImVec4(0.33f, 0.44f, 0.53f, 1.00f);
        colors[ImGuiCol_WindowBg]               = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
        colors[ImGuiCol_ChildBg]                = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
        colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
        colors[ImGuiCol_Border]                 = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
        colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg]                = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
        colors[ImGuiCol_TitleBg]                = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
        colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
        colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
        colors[ImGuiCol_CheckMark]              = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
        colors[ImGuiCol_SliderGrab]             = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
        colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.63f, 0.63f, 0.63f, 1.00f);
        colors[ImGuiCol_Button]                 = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_Header]                 = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.39f, 0.39f, 0.39f, 0.80f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_Separator]              = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
        colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.39f, 0.39f, 0.39f, 0.78f);
        colors[ImGuiCol_SeparatorActive]        = ImVec4(0.66f, 0.66f, 0.66f, 1.00f);
        colors[ImGuiCol_ResizeGrip]             = ImVec4(0.18f, 0.18f, 0.18f, 0.25f);
        colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.39f, 0.39f, 0.39f, 0.67f);
        colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.65f, 0.65f, 0.65f, 0.95f);
        colors[ImGuiCol_Tab]                    = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
        colors[ImGuiCol_TabHovered]             = ImVec4(0.65f, 0.65f, 0.65f, 0.80f);
        colors[ImGuiCol_TabActive]              = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
        colors[ImGuiCol_TabUnfocused]           = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
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
