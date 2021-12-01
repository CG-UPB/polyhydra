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
        m_panels.push_back(m_menu_bar);

        m_mesh_view = new MeshView(720, 480);
        m_mesh_view->set_mesh_object(&m_mesh_obj);

        m_panels.push_back(m_mesh_view);
        m_panels.push_back(m_file_dialog);
        m_panels.push_back(m_custom_ui);
        LogWindow* mylog = LogWindow::getInstance();
        m_panels.push_back(mylog);
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
        meta_mutex.lock();
        open();
        meta_mutex.unlock();

        // Render window forever until window is closed by user
        while (m_window_open)
        {
            // Render single frame
            render();
        }

        meta_mutex.lock();
        close();
        meta_mutex.unlock();
    }

    void Window::close()
    {

        // Destroy Imgui Elements
        for (auto& element: m_panels)
        {
            if (element == LogWindow::getInstance())
                continue;
            delete element;
        }

        if (m_imgui_renderer != nullptr)
            delete m_imgui_renderer;
        m_window_open = false;
    }

    void Window::render()
    {
        meta_mutex.lock();
        // Query whether the window has been closed by the user or not
        bool window_closed = m_imgui_renderer->window_closed();

        if (window_closed)
        {
            std::cout << "Window Closed" << std::endl;
            m_window_open = false;
            meta_mutex.unlock();
            return;
        }
        meta_mutex.unlock();

        rendering_mutex.lock();
        // Pre Render Setup
        m_mesh_obj.m_is_rendering = true;
        m_imgui_renderer->pre_render_step();
        rendering_mutex.unlock();

        // Draw all of our panels
        // The Custom Imgui UI will be run here too
        // Panels are not mutex guarded to avoid deadlocks from the custom imgui function
        for (auto& element: m_panels)
        {
            element->show();
        }
        // After rendering has been done, we can  commit changes to the mesh and other commands

        // Alterations

        // Vertex Highlights
        operation_mutex.lock();
        for(operation_set_highlight highlight : operation_list_vertex_highlights){
            m_mesh_obj.set_highlight(highlight);
        }
        operation_list_vertex_highlights.clear();

        // Shapes

        for(operation_shape shape  : operation_list_shapes){

            ShapePass::add_shape(shape);
        }
        operation_list_shapes.clear();

        operation_mutex.unlock();


        custom_imgui_mutex.lock();
        // Set new Custom UI Function after the previous custom ui function has run to its end
        if(m_new_custom_ui_function_set){
            m_new_custom_ui_function_set = false;

            if(m_temporary_new_custom_ui_function == nullptr)
                m_temporary_new_custom_ui_function = default_callback_function;
            m_custom_ui->set_custom_callback((m_temporary_new_custom_ui_function));
        }
        custom_imgui_mutex.unlock();

        // Post Render Stuff
        rendering_mutex.lock();
        m_imgui_renderer->post_render_step();
        m_mesh_obj.m_is_rendering = false;
        rendering_mutex.unlock();

    }

    // Setter Methods (Programmer to Vos) /////////////////////////////////////////////////////////

    void Window::set_custom_imgui(void_callback vc) {
        custom_imgui_mutex.lock();
        m_temporary_new_custom_ui_function = vc;
        m_new_custom_ui_function_set = true;
        custom_imgui_mutex.unlock();
    }
    void Window::set_vertex_color(OpenVolumeMesh::VertexHandle v_h, bool b, float red, float green, float blue, float alpha)
    {
        operation_mutex.lock();

        std::tuple<OpenVolumeMesh::VertexHandle, float,float,float,float,bool> tuple= std::make_tuple(v_h,red, green, blue,alpha,b);
        operation_set_highlight highlight = tuple;

        operation_list_vertex_highlights.push_back(highlight);

        operation_mutex.unlock();
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

        operation_mutex.lock();

        unsigned int shape_id = shape_id_counter++;

        std::tuple<Shape*, unsigned int, bool> tuple= std::make_tuple(shape, shape_id,true);

        operation_shape shape_operation = tuple;

        operation_list_shapes.push_back(shape_operation);

        operation_mutex.unlock();

        return shape_id;
    }

    void Window::remove_all_highlights(){
        operation_mutex.lock();
        rendering_mutex.lock();
        m_mesh_obj.remove_highlights();
        rendering_mutex.unlock();
        operation_mutex.unlock();
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
        meta_mutex.lock();
        meta_mutex.unlock();
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