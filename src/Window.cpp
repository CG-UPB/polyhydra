//
// Created by jan on 15.10.21.
//

#include "Window.h"
#include <memory>
#include <thread>
#include <mutex>
#include <list>
#include "memory"
#include "input/Input.h"
#include <utility>
#include <OpenVolumeMesh/FileManager/FileManager.hh>

#include "panels/LogWindow.h"
#include "panels/MeshView.h"
#include "ImguiRenderer.h"
#include "panels/MenuBar.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "panels/CustomUIPanel.h"
#include "rendering/passes/ShapePass.h"


namespace vOS
{

    Window& Window::instance()
    {
        static std::mutex s_mutex;
        s_mutex.lock();
        static Window inst;
        s_mutex.unlock();
        return inst;
    }

    Window::Window()
    {
        // Create Custom UI Panel Object
        m_custom_ui = new CustomUIPanel();

        m_file_dialog = new FileDialog();
    }

    Window::~Window()
    {
    }

    void Window::initPanels()
    {
        m_file_dialog = new FileDialog();
        m_menu_bar = new MenuBar();

        m_mesh_view = new MeshView(720, 480);
        m_mesh_view->set_mesh_object(&m_mesh_obj);

        m_log_window = LogWindow::getInstance();
    }

    void Window::open()
    {
        m_window_open = true;
        m_imgui_renderer = new ImguiRenderer(1280, 720, "volumeshOS");

        // Create default UI Panels
        initPanels();

        m_initialized = true;
    }
/*
    void Window::run(void_callback vc)
    {
        set_custom_imgui(vc);

        run();
    }*/

    void Window::run()
    {
        rendering_mutex.lock();
        open();
        rendering_mutex.unlock();

        // Render window forever until window is closed by user
        while (m_window_open)
        {
            // Render single frame
            render();
        }

        rendering_mutex.lock();
        close();
        rendering_mutex.unlock();
    }

    void Window::close()
    {

        // Destroy Imgui Elements
        delete m_file_dialog;
        delete m_menu_bar;
        delete m_mesh_view;
        delete m_custom_ui;


        if (m_imgui_renderer != nullptr)
            delete m_imgui_renderer;
        m_window_open = false;
    }

    void Window::render()
    {
        rendering_mutex.lock();
        // Query whether the window has been closed by the user or not
        bool window_closed = m_imgui_renderer->window_closed();

        if (window_closed)
        {
            std::cout << "Window Closed" << std::endl;
            m_window_open = false;
            rendering_mutex.unlock();
            return;
        }
        rendering_mutex.unlock();

        // Bizarre Observation ???:
        // Removing the rendering mutex from our panels, will result in the dijkstra algorithm to finish immediatly, before even a single render
        // step has been done
        // Having the mutex guard is extremely slow, though everything renders at a nice 60fps, the algorithm takes forever to complete
        // However, outputting anything to the console in either thread significantly increases the algorithm speed without reducing the fps
        // although it will be very abrupt in how many calculations are being done each step

        //static int i;
        //std::cout << "I render "<< i++ << std::endl;
        // Pre Render Setup
        m_mesh_obj.m_is_rendering = true;

        // Do NOT lock the pre render step, Imgui tries to bind to 60 fps which will result in every thread having to adhere to Imgui's fps mechanism
        m_imgui_renderer->pre_render_step();

        rendering_mutex.lock();
        // Draw all of our panels and renderers

        // File Dialog
        m_file_dialog->show();

        // Menu Bar
        m_menu_bar->show();

        // Mesh View
        m_mesh_view->show();

        // Log Window
        m_log_window->show();

        rendering_mutex.unlock();

        // Custom UI
        // Custom UI is not guarded with mutex guards, to avoid self-deadlocking in linear threads / when no threads are used
        m_custom_ui->show();

        rendering_mutex.lock();
        // Set new Custom UI Function after the previous custom ui function has run to its end
        if(m_new_custom_ui_function_set){
            m_new_custom_ui_function_set = false;

            if(m_temporary_new_custom_ui_function == nullptr)
                m_temporary_new_custom_ui_function = default_callback_function;
            m_custom_ui->set_custom_callback((m_temporary_new_custom_ui_function));
        }

        // Post Render Stuff

        m_imgui_renderer->post_render_step();
        m_mesh_obj.m_is_rendering = false;
        rendering_mutex.unlock();

    }

    // Setter Methods (Programmer to Vos) /////////////////////////////////////////////////////////

    void Window::set_custom_imgui(void_callback vc) {
        rendering_mutex.lock();
        m_temporary_new_custom_ui_function = vc;
        m_new_custom_ui_function_set = true;
        rendering_mutex.unlock();
    }
    void Window::highlight_vertex(OpenVolumeMesh::VertexHandle v_h, Color color)
    {
        //static int i;
        rendering_mutex.lock();
        //std::cout << "I did stuff "<< i++ << std::endl;

        Highlight highlight(color, v_h);

        m_mesh_obj.add_highlight(highlight);

        rendering_mutex.unlock();
    }
    void Window::highlight_vertex(OpenVolumeMesh::VertexHandle v_h, float red, float green, float blue, float alpha)
    {
        Color color(red,green,blue,alpha);
        highlight_vertex(v_h, color);
    }

    void Window::remove_vertex_highlight(OpenVolumeMesh::VertexHandle v_h) {
        rendering_mutex.lock();

        m_mesh_obj.remove_highlight((v_h));

        rendering_mutex.unlock();
    }

    void Window::set_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f>* mesh)
    {
        rendering_mutex.lock();
        m_mesh_obj.set_mesh(mesh);
        rendering_mutex.unlock();
    }

    unsigned int Window::add_shape(Shape* shape)
    {
        // TODO Add good exception handling
        if (shape == nullptr)
            return -1;

        // Greedy Approach

        rendering_mutex.lock();

        // Give the shape a unique ID
        unsigned int shape_id = shape_id_counter++;
        shape->set_id((shape_id));

        // Add shape to the ShapePass
        ShapePass::add_shape(shape);
        rendering_mutex.unlock();

        // Command Queue Approach
        return shape_id;
    }

    void Window::remove_all_vertex_highlights(){
        // Greedy Approach

        rendering_mutex.lock();

        m_mesh_obj.remove_highlights();
        rendering_mutex.unlock();
    }

    void Window::remove_shape(unsigned int id){
        // TODO: No method available for this yet
    }

    bool Window::ShowFileDialog(std::string& path, const std::string& extension)
    {

        instance().m_file_dialog->open(extension);
        if (instance().m_file_dialog->is_ok())
        {
            path = instance().m_file_dialog->get_file_path();
            instance().m_file_dialog->set_open(false);
            instance().set_loaded_file_path_name((path));
        }
        return instance().m_file_dialog->is_ok();
    }
    // Read Methods ///////////////////////////////////////////////////////////////////////////////

    bool Window::is_ready()
    {
        rendering_mutex.lock();
        rendering_mutex.unlock();
        return m_initialized;
    }


    bool Window::is_running()
    {
        return m_window_open;
    }


    bool Window::is_closed()
    {
        return is_running();
    }

    /*
    unsigned int Window::add_box(float x, float y, float z, float red, float green, float blue)
    {
        Box* box = new Box(0.05f, 0.05f, 0.05f);
        box->set_transform(glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)));
        box->set_base_color(glm::vec4(red, green, blue, 1.0f));
        return ShapePass::add_shape(box);
    }*/

}