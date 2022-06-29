
#include "Window.h"

#include "input/Input.h"
#include "panels/LogWindow.h"
#include "panels/MeshView.h"
#include "panels/CustomUIPanel.h"
#include "panels/NewFileDialog.h"
#include "rendering/passes/ShapePass.h"

namespace volumeshOS::Internal
{

    Window& Window::instance()
    {
        // Static mutex guard
        static std::mutex s_mutex;
        s_mutex.lock();
        // Creates window instance
        static Window inst;
        // Unlock static mutex guard
        s_mutex.unlock();
        return inst;
    }

    Window::Window()
    {
        // Create Custom UI Panel Object
        m_custom_ui = std::make_unique<CustomUIPanel>();
    }

    void Window::initPanels()
    {
        // Initializes Panels
        m_mesh_view = std::make_unique<MeshView>(720, 480);
        m_log_window = LogWindow::getInstance();
        m_mesh_layer_view = std::make_unique<MeshLayerView>();
        m_toolbar = std::make_unique<ToolBar>();
        m_quality_panel = std::make_unique<QualityPanel>();
    }

    void Window::setup()
    {
        m_window_open = true;
        // Create ImguiRenderer for Imgui communication
        m_imgui_renderer = std::make_unique<ImguiRenderer>(1280, 720, "volumeshOS");

        // Create default UI Panels
        initPanels();

        m_initialized = true;

        // We initialized Vos, now we can activate the corresponding Callback Function, to let the programmer know
        m_vos_initialized();
    }

    void Window::render()
    {
        rendering_mutex.lock();
        // Query whether the window has been closed by the user or not
        bool window_closed = m_imgui_renderer->window_closed();

        if (window_closed)
        {
            // Break render loop in open() method
            m_window_open = false;
            rendering_mutex.unlock();
            return;
        }
        rendering_mutex.unlock();

        // Bizarre Observation:
        // Removing the rendering mutex from our panels, will result in the test algorithm to finish immediatly, before even a single render
        // step has been done
        // Having the mutex guard is extremely slow, though everything renders at a nice 60fps, the algorithm takes forever to complete
        // However, outputting anything to the console in either thread significantly increases the algorithm speed without reducing the fps
        // although it will be very abrupt in how many calculations are being done each step

        // Pre Render Setup

        // Do NOT lock the pre render step, Imgui tries to bind to 60 fps which will result in every thread having to adhere to Imgui's fps mechanism
        m_imgui_renderer->pre_render_step();

        rendering_mutex.lock();

        // Update ui color mode if necessary
        if (m_update_ui_color_mode)
        {
            if (m_ui_color_mode == UI_COLOR_MODE_LIGHT)
            {
                m_imgui_renderer->load_light_mode();
                m_mesh_view->m_renderer->m_background_pass.set_background_color({1.0f, 1.0f, 1.0f, 1.0f});
            }
            else if (m_ui_color_mode == UI_COLOR_MODE_DARK)
            {
                m_imgui_renderer->load_dark_mode();
                m_mesh_view->m_renderer->m_background_pass.set_background_color({0.2f, 0.2f, 0.2f, 1.0f});
            }
            m_update_ui_color_mode = false;
        }

        // Draw all of our panels and renderers

        // Mesh volumeshOS
        m_mesh_view->show();

        // Log Window
        m_log_window->show();

        // QualityPanel
        m_quality_panel->show();

        // ToolBar
        m_toolbar->show();

        // MeshLayerView
        m_mesh_layer_view->show();

        rendering_mutex.unlock();

        //ImGui::ShowDemoWindow();

        // Custom UI is not guarded with mutex guards, to avoid self-deadlocking in linear threads / when no threads are used
        m_custom_ui->show();

        rendering_mutex.lock();
        // Set new Custom UI Function after the previous custom ui function has run to its end
        if (m_new_custom_ui_function_set)
        {
            m_new_custom_ui_function_set = false;

            // Set to default if no actual function has been set
            if (m_temporary_new_custom_ui_function == nullptr)
                m_temporary_new_custom_ui_function = default_callback_function;
            m_custom_ui->set_custom_callback((m_temporary_new_custom_ui_function));
        }

        // Post Render Step
        m_imgui_renderer->post_render_step();

        rendering_mutex.unlock();

    }

    void Window::set_keybind_manual(int glfw_key_from, int glfw_key_to)
    {
        rendering_mutex.lock();
        Input::set_keybind(glfw_key_from, glfw_key_to);
        rendering_mutex.unlock();
    }

    void Window::set_intepret_input(bool interpret)
    {
        rendering_mutex.lock();
        Input::accept_input(interpret);
        rendering_mutex.unlock();
    }


    void Window::set_custom_imgui(void_callback vc)
    {
        rendering_mutex.lock();
        // Set Temporary Function
        m_temporary_new_custom_ui_function = std::move(vc);
        m_new_custom_ui_function_set = true;
        rendering_mutex.unlock();
    }


    void Window::take_screenshot(const std::string& filepath)
    {
        rendering_mutex.lock();
        this->m_mesh_view->m_take_screenshot(filepath);
        rendering_mutex.unlock();
    }

    char const * openFileDialog(const char * filedialog)
    {
        NewFileDialog file_dialog;

        return file_dialog.openDialog(filedialog);
    }

    char const * saveFileDialog(const char * filedialog)
    {
        NewFileDialog file_dialog;

        return file_dialog.saveDialog(filedialog);
    }

    void Window::remove_all_shapes(){
        rendering_mutex.lock();

        ShapePass::remove_all();
        rendering_mutex.unlock();
    }

    void Window::remove_shape(unsigned int id)
    {
        rendering_mutex.lock();
        ShapePass::remove_shape(id);
        rendering_mutex.unlock();
    }

    unsigned int Window::add_shape(Shape* shape)
    {
        // Don't add a non-existing shape
        if (shape == nullptr)
            return -1;

        rendering_mutex.lock();

        // Give the shape a unique ID
        unsigned int shape_id = shape_id_counter++;
        shape->set_id((shape_id));
        // Add shape to the ShapePass
        ShapePass::add_shape(shape);
        rendering_mutex.unlock();

        return shape_id;
    }

    void Window::save_mesh_data(int mesh_id, const std::string& json_file_path)
    {
        rendering_mutex.lock();

        // Get MeshObject
        auto mesh_obj = get_mesh_obj(mesh_id);
        // Save MeshObject Data
        if (mesh_obj != nullptr)
        {
            try {


                // Create Json Object from MeshData
                nlohmann::json j = mesh_obj->get_data().to_json();

                // Stream it into file
                std::ofstream o(json_file_path);
                o << j << std::endl;

                // Close stream
                o.close();
            }catch(std::exception e){
                std::cout << " Error saving " << json_file_path << std::endl;
            }

        }

        rendering_mutex.unlock();
    }

    void Window::save_mesh_data(int mesh_id)
    {
        std::string filename;

        rendering_mutex.lock();

        // Get MeshObject
        auto mesh_obj = get_mesh_obj(mesh_id);
        // Get MeshObject name
        if (mesh_obj != nullptr)
        {
            filename = mesh_obj->get_mesh_name();
        }else{
            rendering_mutex.unlock();
            return;
        }
        rendering_mutex.unlock();

        save_mesh_data(mesh_id, filename);

    }

    void Window::load_mesh_data(int mesh_id)
    {
        std::string filename;

        rendering_mutex.lock();

        // Get MeshObject
        auto mesh_obj = get_mesh_obj(mesh_id);
        // Get MeshObject name
        if (mesh_obj != nullptr)
        {
            filename = mesh_obj->get_mesh_name();
        }else{
            rendering_mutex.unlock();
            return;
        }
        rendering_mutex.unlock();

        load_mesh_data(mesh_id, filename);

    }

    void Window::load_mesh_data(int mesh_id, const std::string& json_file_path)
    {
        rendering_mutex.lock();

        // Get MeshObject
        auto mesh_obj = get_mesh_obj(mesh_id);
        // Save MeshObject Data
        if (mesh_obj != nullptr)
        {
            try{
                // Stream from file to JSON
                std::ifstream i(json_file_path);
                nlohmann::json j;
                i >> j;

                // Read and set Json Data
                mesh_obj->get_data().load_from_json(j);

                // Close stream
                i.close();
            }catch(std::exception& e){
                std::cout << " Error loading " << json_file_path << std::endl;
            }
        }

        rendering_mutex.unlock();
    }

    void Window::load_light_mode()
    {
        m_ui_color_mode = UI_COLOR_MODE_LIGHT;
        m_update_ui_color_mode = true;
    }

    void Window::load_dark_mode()
    {
        m_ui_color_mode = UI_COLOR_MODE_DARK;
        m_update_ui_color_mode = true;
    }

    // Read Methods ///////////////////////////////////////////////////////////////////////////////

    bool Window::is_ready() const
    {
        return m_initialized;
    }


    bool Window::is_closed() const
    {
        return !m_window_open;
    }

}