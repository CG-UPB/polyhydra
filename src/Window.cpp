//
// Created by jan on 15.10.21.
//

#include "Window.h"
#include <memory>
#include <mutex>
#include <list>
#include "input/Input.h"
#include <utility>
#include <OpenVolumeMesh/FileManager/FileManager.hh>

#include "panels/LogWindow.h"
#include "panels/MeshView.h"
#include "ImguiRenderer.h"
#include "panels/CustomUIPanel.h"
#include "rendering/passes/ShapePass.h"
#include "panels/NewFileDialog.h"

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
    }

    Window::~Window()
    {
    }

    void Window::initPanels()
    {
        m_mesh_view = new MeshView(720, 480);
        m_log_window = LogWindow::getInstance();
        m_mesh_layer_view = new MeshLayerView();
        m_toolbar = new ToolBar();
    }

    void Window::setup()
    {
        m_window_open = true;
        m_imgui_renderer = new ImguiRenderer(1280, 720, "volumeshOS");

        // Create default UI Panels
        initPanels();

        m_initialized = true;

        // We initialized Vos, now we can activate the corresponding Callback Function, to let the programmer know
        m_vos_initialized();
    }

    void Window::run()
    {
        // We should not allow calling this method while it is already running
        if(m_is_in_render_loop)
            return;

        m_is_in_render_loop = true;
        setup();

        // Render window forever until window is closed by user
        while (m_window_open)
        {
            // Render single frame
            render();
        }

        close();

        m_is_in_render_loop= false;
    }

    void Window::close()
    {
        // Destroy all meshes
        remove_all_meshes();

        rendering_mutex.lock();
        // Destroy Imgui Elements
        delete m_mesh_view;
        delete m_custom_ui;


        if (m_imgui_renderer != nullptr)
            delete m_imgui_renderer;
        m_window_open = false;
        rendering_mutex.unlock();
    }

    void Window::end(){
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

        // Do NOT lock the pre render step, Imgui tries to bind to 60 fps which will result in every thread having to adhere to Imgui's fps mechanism
        m_imgui_renderer->pre_render_step();

        rendering_mutex.lock();
        // Draw all of our panels and renderers

        // Mesh View
        m_mesh_view->show();

        // Log Window
        m_log_window->show();

        // ToolBar
        m_toolbar->show();

        // MeshLayerView
        m_mesh_layer_view->show();

        rendering_mutex.unlock();

        // Custom UI
        // Custom UI is not guarded with mutex guards, to avoid self-deadlocking in linear threads / when no threads are used
        m_custom_ui->show();

        rendering_mutex.lock();
        // Set new Custom UI Function after the previous custom ui function has run to its end
        if (m_new_custom_ui_function_set)
        {
            m_new_custom_ui_function_set = false;

            if (m_temporary_new_custom_ui_function == nullptr)
                m_temporary_new_custom_ui_function = default_callback_function;
            m_custom_ui->set_custom_callback((m_temporary_new_custom_ui_function));
        }

        // Post Render Stuff

        m_imgui_renderer->post_render_step();
        rendering_mutex.unlock();

    }

    // Setter Methods (Programmer to Vos) /////////////////////////////////////////////////////////

    void Window::select_element(int mesh_id, int element_handle_id, int element_type)
    {
        rendering_mutex.lock();
        auto mesh = get_mesh_obj(mesh_id);

        if (mesh != nullptr)
            mesh->select_element(element_handle_id, element_type);
        rendering_mutex.unlock();

        // Call the Selection Callback Function
        if (element_type == 0)
        {
            m_on_face_selection(mesh_id, element_handle_id, true);
        }
        else if (element_type == 1)
        {
            m_on_vertex_selection(mesh_id, element_handle_id, true);
        }
        else if (element_type == 2)
        {
            m_on_edge_selection(mesh_id, element_handle_id, true);
        }
        else
            m_on_cell_selection(mesh_id, element_handle_id, true);

    }

    void Window::unselect_element(int mesh_id, int element_handle_id, int element_type)
    {
        rendering_mutex.lock();

        auto mesh = get_mesh_obj(mesh_id);

        if (mesh != nullptr)
            mesh->unselect_element(element_handle_id, element_type);

        rendering_mutex.unlock();

        // Call the Selection Callback Function
        if (element_type == 0)
        {
            m_on_face_selection(mesh_id, element_handle_id, false);
        }
        else if (element_type == 1)
        {
            m_on_vertex_selection(mesh_id, element_handle_id, false);
        }
        else if (element_type == 2)
        {
            m_on_edge_selection(mesh_id, element_handle_id, false);
        }
        else
            m_on_cell_selection(mesh_id, element_handle_id, false);

    }

    void Window::unselect_all_elements(int mesh_id)
    {
        rendering_mutex.lock();
        MeshObject* mesh = get_mesh_obj(mesh_id);

        if (mesh != nullptr)
            mesh->unselect_all();
        rendering_mutex.unlock();
    }

    void Window::unselect_all_elements()
    {
        rendering_mutex.lock();
        for (std::pair<int, MeshObject*> element: m_mesh_objects)
        {
            element.second->unselect_all();
        }
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

    void Window::set_mesh_rendering_mode(std::string mode)
    {
        set_mesh_rendering_mode(0, mode);
    }

    void Window::set_mesh_rendering_mode(int mesh_id, std::string mode)
    {
        rendering_mutex.lock();

        // Change Mesh Settings
        MeshObject* mesh_obj = get_mesh_obj(mesh_id);
        if (mesh_obj != nullptr)
        {
            mesh_obj->get_data().rendering_mode = std::move(mode);
        }

        rendering_mutex.unlock();
    }

    std::string Window::get_mesh_rendering_mode(int mesh_id)
    {
        MeshObject* mesh_obj = get_mesh_obj(mesh_id);
        if (mesh_obj != nullptr)
        {
            auto data = mesh_obj->get_data();
            return data.rendering_mode;
        }
        return "";
    }

    void Window::set_mesh_color(Color color)
    {
        set_mesh_color(0, color);
    }

    void Window::set_mesh_color(int mesh_id, Color color)
    {
        rendering_mutex.lock();

        // Change Mesh Settings
        MeshObject* mesh_obj = get_mesh_obj(mesh_id);
        if (mesh_obj != nullptr)
        {
            mesh_obj->get_data().m_color = color;
        }

        rendering_mutex.unlock();
    }

    Color Window::get_mesh_color(int mesh_id)
    {
        MeshObject* mesh_obj = get_mesh_obj(mesh_id);
        if (mesh_obj != nullptr)
        {
            auto data = mesh_obj->get_data();
            return data.m_color;
        }
        return {1.0f, 1.0f, 1.0f};
    }

    void Window::set_mesh_visibility(bool visible)
    {
        set_mesh_visibility(0, visible);
    }

    void Window::set_mesh_visibility(int mesh_id, bool visible)
    {
        rendering_mutex.lock();

        // Change Mesh Settings
        MeshObject* mesh_obj = get_mesh_obj(mesh_id);
        if (mesh_obj != nullptr)
        {
            mesh_obj->get_data().m_visible = visible;
        }

        rendering_mutex.unlock();
    }

    bool Window::get_mesh_visibility(int mesh_id)
    {
        MeshObject* mesh_obj = get_mesh_obj(mesh_id);
        if (mesh_obj != nullptr)
        {
            auto data = mesh_obj->get_data();
            return data.m_visible;
        }
        return false;
    }

    void Window::set_mesh_slice_level(float slice_level)
    {
        set_mesh_slice_level(0, slice_level);
    }

    void Window::set_mesh_slice_level(int mesh_id, float slice_level)
    {
        rendering_mutex.lock();

        // Change Mesh Settings
        MeshObject* mesh_obj = get_mesh_obj(mesh_id);
        if (mesh_obj != nullptr)
        {
            mesh_obj->get_data().m_slice_level = slice_level;
        }

        rendering_mutex.unlock();
    }

    float Window::get_mesh_slice_level(int mesh_id)
    {
        MeshObject* mesh_obj = get_mesh_obj(mesh_id);
        if (mesh_obj != nullptr)
        {
            auto data = mesh_obj->get_data();
            return data.m_slice_level;
        }
        return false;
    }

    void Window::set_mesh_slice_locked(int mesh_id, bool locked)
    {
        rendering_mutex.lock();

        // Change Mesh Settings
        MeshObject* mesh_obj = get_mesh_obj(mesh_id);
        if (mesh_obj != nullptr)
        {
            mesh_obj->get_data().m_slice_locked = locked;
        }

        rendering_mutex.unlock();
    }

    bool Window::get_mesh_slice_locked(int mesh_id)
    {
        MeshObject* mesh_obj = get_mesh_obj(mesh_id);
        if (mesh_obj != nullptr)
        {
            auto data = mesh_obj->get_data();
            return data.m_slice_locked;
        }
        return false;
    }

    void Window::set_mesh_peel_level(int peel_level)
    {
        set_mesh_slice_level(0, peel_level);
    }

    void Window::set_mesh_peel_level(int mesh_id, int peel_level)
    {
        rendering_mutex.lock();

        // Change Mesh Settings
        MeshObject* mesh_obj = get_mesh_obj(mesh_id);
        if (mesh_obj != nullptr)
        {
            mesh_obj->get_data().m_peel_level = peel_level;
        }

        rendering_mutex.unlock();
    }

    int Window::get_mesh_peel_level(int mesh_id)
    {
        MeshObject* mesh_obj = get_mesh_obj(mesh_id);
        if (mesh_obj != nullptr)
        {
            auto data = mesh_obj->get_data();
            return data.m_peel_level;
        }
        return false;
    }

    void Window::set_mesh_cell_size(float cell_size)
    {
        set_mesh_slice_level(0, cell_size);
    }

    void Window::set_mesh_cell_size(int mesh_id, float cell_size)
    {
        rendering_mutex.lock();

        // Change Mesh Settings
        MeshObject* mesh_obj = get_mesh_obj(mesh_id);
        if (mesh_obj != nullptr)
        {
            mesh_obj->get_data().m_cell_size = cell_size;
        }

        rendering_mutex.unlock();
    }

    float Window::get_mesh_cell_size(int mesh_id)
    {
        MeshObject* mesh_obj = get_mesh_obj(mesh_id);
        if (mesh_obj != nullptr)
        {
            auto data = mesh_obj->get_data();
            return data.m_cell_size;
        }
        return false;
    }

    void Window::set_mesh_position(int mesh_id, float x, float y, float z)
    {
        rendering_mutex.lock();

        // Change Mesh Settings
        MeshObject* mesh_obj = get_mesh_obj(mesh_id);
        if (mesh_obj != nullptr)
        {
            mesh_obj->get_data().position = glm::vec3(x, y, z);
        }

        rendering_mutex.unlock();
    }

    void Window::set_mesh_scale(int mesh_id, float scale)
    {
        rendering_mutex.lock();

        // Change Mesh Settings
        MeshObject* mesh_obj = get_mesh_obj(mesh_id);
        if (mesh_obj != nullptr)
        {
            mesh_obj->get_data().scale = glm::vec3(scale, scale, scale);
        }

        rendering_mutex.unlock();
    }

    void Window::set_focus_mesh(int mesh_id)
    {
        rendering_mutex.lock();

        // Change Mesh Settings
        MeshObject* mesh_obj = get_mesh_obj(mesh_id);
        if (mesh_obj != nullptr)
        {
            m_mesh_view->set_zoom_point(mesh_obj->get_mesh_offset());
        }

        rendering_mutex.unlock();
    }

    void Window::set_custom_imgui(void_callback vc)
    {
        rendering_mutex.lock();
        m_temporary_new_custom_ui_function = vc;
        m_new_custom_ui_function_set = true;
        rendering_mutex.unlock();
    }

    std::vector<int>* Window::get_all_mesh_ids() {
        std::vector<int>* ids = new std::vector<int>();

        for(auto pair : m_mesh_objects){
            ids->push_back(pair.first);
        }

        return ids;
    }

    void Window::highlight_vertex(int mesh_id, OpenVolumeMesh::VertexHandle v_h, Color color)
    {
        rendering_mutex.lock();

        Highlight highlight(color, v_h);

        if (get_mesh_obj(mesh_id) != nullptr)
            get_mesh_obj(mesh_id)->add_highlight(highlight);

        rendering_mutex.unlock();
    }

    void Window::highlight_vertex(int mesh_id, OpenVolumeMesh::VertexHandle v_h, float red, float green, float blue,
                                  float alpha)
    {
        Color color(red, green, blue, alpha);
        highlight_vertex(mesh_id, v_h, color);
    }

    void Window::remove_vertex_highlight(int mesh_id, OpenVolumeMesh::VertexHandle v_h)
    {
        rendering_mutex.lock();

        if (get_mesh_obj(mesh_id) != nullptr)
            get_mesh_obj(mesh_id)->remove_highlight((v_h));

        rendering_mutex.unlock();
    }


    void Window::set_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f>* mesh, int index)
    {
        rendering_mutex.lock();
        auto* mesh_obj = new MeshObject();
        mesh_obj->set_data(MeshData());
        mesh_obj->set_mesh(mesh);
        std::cout << "Adding mesh with id " << index << std::endl;

        // check if index of mesh already exist: yes -> replace it, no -> just insert it
        auto search = m_mesh_objects.find(index);
        if (search != m_mesh_objects.end())
        {
            // delete old pointer
            delete search->second;
            search->second = mesh_obj;
        }
        else
        {
            m_mesh_objects.emplace(index, mesh_obj);
        }
        if (m_mesh_objects.size() == 1)
            set_mesh_active(index);

        calculate_selection_offsets();
        rendering_mutex.unlock();
    }

    int Window::add_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f>* mesh)
    {
        rendering_mutex.lock();
        int index = m_total_number_of_loaded_meshes++;

        // Create Mesh Object
        auto* mesh_obj = new MeshObject();
        mesh_obj->set_data(MeshData());
        mesh_obj->set_mesh(mesh);

        // Add mesh to our map
        m_mesh_objects.emplace(index, mesh_obj);

        // Make the mesh the active mesh, if it is the only one available
        if (m_mesh_objects.size() == 1)
            set_mesh_active(index);

        calculate_selection_offsets();
        rendering_mutex.unlock();
        return index;
    }

    void Window::remove_mesh(int index)
    {
        rendering_mutex.lock();
        // Get Mesh Object
        auto* mesh_obj = get_mesh_obj(index);
        if (mesh_obj == nullptr)
        {
            // Mesh Object does not exist at given index
            return;
        }

        // Update Active Mesh
        bool was_active_mesh = m_active_mesh == index;
        if (was_active_mesh)
        {
            int new_active_mesh = -1;
            // Find the first element in our map
            for (auto obj: m_mesh_objects)
            {
                new_active_mesh = obj.first;
                break;
            }
            // Set new active mesh
            set_mesh_active(new_active_mesh);
        }

        // Delete from our Map
        auto iterator = m_mesh_objects.find(index);

        m_mesh_objects.erase(iterator);

        delete mesh_obj;

        rendering_mutex.unlock();
    }

    void Window::remove_all_meshes(){
        rendering_mutex.lock();

        std::vector<int> all_ids;

        // Iterate all Meshes to get their IDs
        for(auto obj : m_mesh_objects){
            all_ids.push_back(obj.first);
        }

        // Delete all Meshes by their ID
        for(auto id : all_ids){
            rendering_mutex.unlock();
            remove_mesh(id);
            rendering_mutex.lock();
        }

        rendering_mutex.unlock();
    };

    void Window::calculate_selection_offsets()
    {
        int offset = 0;
        for (const auto& mesh_obj: m_mesh_objects)
        {
            mesh_obj.second->set_selection_offset(offset);
            offset = std::get<1>(mesh_obj.second->selection_offset()) + 1;
        }
    }

    void Window::set_mesh_active(int index)
    {
        if(index < 0)
            return;
        auto search = m_mesh_objects.find(index);
        if (search != m_mesh_objects.end())
        {
            m_active_mesh = index;
        }
    }

    int Window::get_mesh_active()
    {
        return m_active_mesh;
    }

    MeshObject* Window::get_mesh_obj(int index)
    {
        auto search = m_mesh_objects.find(index);
        if (search != m_mesh_objects.end())
        {
            return search->second;
        }
        else
        {
            //std::cout << "Could not find mesh object with index " << index << std::endl;
            return nullptr;
        }
    }

    void Window::remove_all_vertex_highlights()
    {
        rendering_mutex.lock();

        for (auto it = m_mesh_objects.begin(); it != m_mesh_objects.end(); it++)
        {
            it->second->remove_highlights();
        }
        rendering_mutex.unlock();
    }


    void Window::take_screenshot(std::string filename)
    {
        rendering_mutex.lock();
        this->m_mesh_view->m_take_screenshot(filename);
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
        // TODO Add good exception handling
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
    void Window::camera_set_position(float x, float y, float z)
    {
        rendering_mutex.lock();

        rendering_mutex.unlock();
    }

    // Read Methods ///////////////////////////////////////////////////////////////////////////////


    bool Window::has_mesh()
    {
        return !m_mesh_objects.empty();
    }

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

}