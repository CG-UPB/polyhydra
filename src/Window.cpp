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
#include "iostream"

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
        m_custom_ui = new CustomUIPanel();
    }

    Window::~Window()
    {
        // Empty
    }

    void Window::initPanels()
    {
        // Initializes Panels
        m_mesh_view = new MeshView(720, 480);
        m_log_window = LogWindow::getInstance();
        m_mesh_layer_view = new MeshLayerView();
        m_toolbar = new ToolBar();
    }

    void Window::setup()
    {
        m_window_open = true;
        // Create ImguiRenderer for Imgui communication
        m_imgui_renderer = new ImguiRenderer(1280, 720, "volumeshOS");

        // Create default UI Panels
        initPanels();

        m_initialized = true;

        // We initialized Vos, now we can activate the corresponding Callback Function, to let the programmer know
        m_vos_initialized();
    }

    void Window::open()
    {
        // We should not allow calling this method while it is already running
        if(m_is_in_render_loop)
            return;

        m_is_in_render_loop = true;
        // Setup Vos
        setup();

        // Render window forever until window is closed by user
        while (m_window_open)
        {
            // Render single frame
            render();
        }

        // Close Vos
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
        // Breaks render loop in open() method
        m_window_open = false;
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

        //ImGui::ShowDemoWindow();

        // Custom UI
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

    // Setter Methods (Programmer to Vos) /////////////////////////////////////////////////////////

    void Window::select_element(int mesh_id, int element_handle_id, int element_type)
    {
        rendering_mutex.lock();
        // Get MeshObject
        auto mesh = get_mesh_obj(mesh_id);

        // Select desired element
        if (mesh != nullptr)
            mesh->select_element(element_handle_id, element_type);
        rendering_mutex.unlock();

        // Call the Selection Callback Function
        if (element_type == 3)
        {
            std::cout << "Ja\n";
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

        // Get MeshObject
        auto mesh = get_mesh_obj(mesh_id);

        // Unselect desired element
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
        // Get MeshObject
        MeshObject* mesh = get_mesh_obj(mesh_id);

        // Unselect all elements
        if (mesh != nullptr)
            mesh->unselect_all();
        rendering_mutex.unlock();
    }

    void Window::unselect_all_elements()
    {
        rendering_mutex.lock();
        // Unselect all elements of all meshes
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
        // Change MeshObject Data
        get_mesh_obj(mesh_id)->get_data().m_rendering_mode = std::move(mode);
    }

    std::string Window::get_mesh_rendering_mode(int mesh_id)
    {
        // Get MeshObject Data
        return get_mesh_obj(mesh_id)->get_data().m_rendering_mode;
    }

    void Window::set_mesh_color(Color color)
    {
        set_mesh_color(0, color);
    }

    void Window::set_mesh_color(int mesh_id, Color color)
    {
        // Change MeshObject Data
        get_mesh_obj(mesh_id)->get_data().m_color = color;
    }

    Color Window::get_mesh_color(int mesh_id)
    {
        // Get MeshObject Data
        return get_mesh_obj(mesh_id)->get_data().m_color;
    }


    void Window::set_mesh_selection_color(int mesh_id, Color color)
    {
        // Change MeshObject Data
        get_mesh_obj(mesh_id)->get_data().m_selection_color = color;
    }

    Color Window::get_mesh_selection_color(int mesh_id)
    {
        // Get MeshObject Data
        return get_mesh_obj(mesh_id)->get_data().m_selection_color;
    }

    void Window::set_mesh_visibility(bool visible)
    {
        // Change MeshObject Data
        set_mesh_visibility(0, visible);
    }

    void Window::set_mesh_visibility(int mesh_id, bool visible)
    {
        // Change MeshObject Data
        get_mesh_obj(mesh_id)->get_data().m_visible = visible;
    }

    bool Window::get_mesh_visibility(int mesh_id)
    {
        // Get MeshObject Data
        return get_mesh_obj(mesh_id)->get_data().m_visible;
    }

    void Window::set_mesh_slice_level(float slice_level)
    {
        set_mesh_slice_level(0, slice_level);
    }

    void Window::set_mesh_slice_level(int mesh_id, float slice_level)
    {
        // Change MeshObject Data
            get_mesh_obj(mesh_id)->get_data().m_slice_level = slice_level;
    }

    float Window::get_mesh_slice_level(int mesh_id)
    {
        // Get MeshObject Data
        return get_mesh_obj(mesh_id)->get_data().m_slice_level;
    }

    void Window::set_mesh_slice_locked(int mesh_id, bool locked)
    {
        // Change MeshObject Data
            get_mesh_obj(mesh_id)->get_data().m_slice_locked = locked;
    }

    bool Window::get_mesh_slice_locked(int mesh_id)
    {
        // Get MeshObject Data
        return get_mesh_obj(mesh_id)->get_data().m_slice_locked;
    }

    void Window::set_mesh_peel_level(int peel_level)
    {
        set_mesh_peel_level(0, peel_level);
    }

    void Window::set_mesh_peel_level(int mesh_id, int peel_level)
    {
        // Change MeshObject Data
        get_mesh_obj(mesh_id)->get_data().m_peel_level = peel_level;
    }

    int Window::get_mesh_peel_level(int mesh_id)
    {
        // Get MeshObject Data
        return get_mesh_obj(mesh_id)->get_data().m_peel_level;
    }

    void Window::set_mesh_cell_size(float cell_size)
    {
        set_mesh_cell_size(0, cell_size);
    }

    void Window::set_mesh_cell_size(int mesh_id, float cell_size)
    {
        // Change MeshObject Data
        get_mesh_obj(mesh_id)->get_data().m_cell_size = cell_size;
    }

    float Window::get_mesh_cell_size(int mesh_id)
    {
        // Get MeshObject Data
        return get_mesh_obj(mesh_id)->get_data().m_cell_size;
    }


    void Window::set_mesh_specular_exponent(int mesh_id, float exp)
    {
        get_mesh_obj(mesh_id)->get_data().m_specular_exponent = exp;
    }

    float Window::get_mesh_specular_exponent(int mesh_id)
    {
        return get_mesh_obj(mesh_id)->get_data().m_specular_exponent;
    }

    void Window::set_mesh_specular_strength(int mesh_id, float strength)
    {
        get_mesh_obj(mesh_id)->get_data().m_specular_strength = strength;
    }

    float Window::get_mesh_specular_strength(int mesh_id)
    {
        return get_mesh_obj(mesh_id)->get_data().m_specular_strength;
    }

    void Window::set_mesh_ambient_strength(int mesh_id, float strength)
    {
        get_mesh_obj(mesh_id)->get_data().m_ambient_strength = strength;
    }

    float Window::get_mesh_ambient_strength(int mesh_id)
    {
        return get_mesh_obj(mesh_id)->get_data().m_ambient_strength;
    }

    void Window::set_mesh_diffuse_strength(int mesh_id, float strength)
    {
        get_mesh_obj(mesh_id)->get_data().m_diffuse_strength = strength;
    }

    float Window::get_mesh_diffuse_strength(int mesh_id)
    {
        return get_mesh_obj(mesh_id)->get_data().m_diffuse_strength;
    }

    void Window::set_mesh_position(int mesh_id, float x, float y, float z)
    {
        // Change MeshObject Data
        get_mesh_obj(mesh_id)->get_data().m_position = glm::vec3(x, y, z);
    }

    void Window::set_mesh_scale(int mesh_id, float scale)
    {
        // Change MeshObject Data
        get_mesh_obj(mesh_id)->get_data().m_scale = glm::vec3(scale, scale, scale);
    }


    void Window::set_custom_imgui(void_callback vc)
    {
        rendering_mutex.lock();
        // Set Temporary Function
        m_temporary_new_custom_ui_function = vc;
        m_new_custom_ui_function_set = true;
        rendering_mutex.unlock();
    }

    std::vector<int>* Window::get_all_mesh_ids() {
        // Create ID vector
        std::vector<int>* ids = new std::vector<int>();

        // Copy all existing indizes into the ID vector
        for(auto pair : m_mesh_objects){
            ids->push_back(pair.first);
        }

        return ids;
    }


    void Window::set_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* mesh, int index)
    {
        rendering_mutex.lock();
        // Create MeshObject
        auto* mesh_obj = new MeshObject();
        mesh_obj->set_mesh_name(std::to_string(mesh->n_vertices()));
        // Set its Data
        mesh_obj->set_data(MeshData());
        // Set its Mesh
        mesh_obj->set_mesh(mesh);

        // Check if index of mesh already exist: yes -> replace it, no -> just insert it
        auto search = m_mesh_objects.find(index);
        if (search != m_mesh_objects.end())
        {
            // Delete old pointer
            delete search->second;
            search->second = mesh_obj;
        }
        else
        {
            // Insert Mesh
            m_mesh_objects.emplace(index, mesh_obj);
        }

        // If no other Mesh exists, focus the newly added Mesh
        if (m_mesh_objects.size() == 1) {
            rendering_mutex.unlock();
            set_mesh_focus(index);
            rendering_mutex.lock();
        }
        // Calculate Offsets
        int offset = 0;
        for (const auto& mesh_obj: m_mesh_objects)
        {
            mesh_obj.second->set_selection_offset(offset);
            offset = std::get<1>(mesh_obj.second->selection_offset()) + 1;
        }
        rendering_mutex.unlock();
    }

    int Window::add_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* mesh)
    {
        rendering_mutex.lock();
        // Generate Index
        int index = m_total_number_of_loaded_meshes++;

        // Create MeshObject
        auto* mesh_obj = new MeshObject();
        mesh_obj->set_mesh_name(std::to_string(mesh->n_vertices()));
        mesh_obj->set_data(MeshData());
        mesh_obj->set_mesh(mesh);

        // Add mesh to our map
        m_mesh_objects.emplace(index, mesh_obj);

        // If no other Mesh exists, focus the newly added Mesh
        if (m_mesh_objects.size() == 1) {

            rendering_mutex.unlock();
            set_mesh_focus(index);
            rendering_mutex.lock();
        }

        // Calculate Offsets
        int offset = 0;
        for (const auto& mesh_obj: m_mesh_objects)
        {
            mesh_obj.second->set_selection_offset(offset);
            offset = std::get<1>(mesh_obj.second->selection_offset()) + 1;
        }
        rendering_mutex.unlock();
        return index;
    }

    void Window::remove_mesh(int index)
    {
        rendering_mutex.lock();
        // Get MeshObject
        auto* mesh_obj = get_mesh_obj(index);

        if (mesh_obj == nullptr)
        {
            // Mesh Object does not exist at given index
            rendering_mutex.unlock();
            return;
        }

        // Update Active Mesh
        bool was_active_mesh = m_focused_mesh == index;
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
            rendering_mutex.unlock();
            set_mesh_focus(index);
            rendering_mutex.lock();
        }

        // Delete from our Map
        auto iterator = m_mesh_objects.find(index);

        m_mesh_objects.erase(iterator);

        // Delete Object
        delete mesh_obj;

        rendering_mutex.unlock();
    }

    void Window::remove_all_meshes(){
        rendering_mutex.lock();

        std::vector<int> all_ids;

        // Iterate all Meshes to get_rgb their IDs
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


    void Window::set_mesh_focus(int index)
    {
        if(index < 0)
            return;

        rendering_mutex.lock();
        // Get MeshObject
        auto search = m_mesh_objects.find(index);
        if (search != m_mesh_objects.end())
        {
            m_focused_mesh = index;
        }
        // Focus Camera
        MeshObject* mesh_obj = get_mesh_obj(m_focused_mesh);
        if (mesh_obj != nullptr)
        {
            //m_mesh_view->set_zoom_point(mesh_obj->get_mesh_offset());
        }
        rendering_mutex.unlock();
    }

    int Window::get_mesh_focus()
    {
        return m_focused_mesh;
    }

    MeshObject* Window::get_mesh_obj(int index)
    {
        return m_mesh_objects[index];
    }


    void Window::take_screenshot(std::string filepath)
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

    void Window::camera_set_position(float x, float y, float z)
    {
        rendering_mutex.lock();
        m_mesh_view->m_render_data.camera.position = glm::vec3(x,y,z);
        rendering_mutex.unlock();
    }

    void Window::camera_focus_on(int mesh_id, int ovm_face_id)
    {
        rendering_mutex.lock();
        // Get Face Normal
        auto face_normal = Window::instance().get_mesh_obj(mesh_id)->get_mvb()->get_face_normal(ovm_face_id);
        auto face_barycenter = Window::instance().get_mesh_obj(mesh_id)->get_mvb()->get_face_barycenter(ovm_face_id);
        // Focus
        m_mesh_view->m_render_data.camera.focus_spot(face_barycenter, face_normal);
        rendering_mutex.unlock();
    }

    void Window::save_mesh_data(int mesh_id, std::string json_file_path)
    {
        rendering_mutex.lock();

        // Get MeshObject
        MeshObject* mesh_obj = get_mesh_obj(mesh_id);
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
        std::string filename = "";

        rendering_mutex.lock();

        // Get MeshObject
        MeshObject* mesh_obj = get_mesh_obj(mesh_id);
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
        std::string filename = "";

        rendering_mutex.lock();

        // Get MeshObject
        MeshObject* mesh_obj = get_mesh_obj(mesh_id);
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

    void Window::load_mesh_data(int mesh_id, std::string json_file_path)
    {
        rendering_mutex.lock();

        // Get MeshObject
        MeshObject* mesh_obj = get_mesh_obj(mesh_id);
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
            }catch(std::exception e){
                std::cout << " Error loading " << json_file_path << std::endl;
            }
        }

        rendering_mutex.unlock();
    }

    // Read Methods ///////////////////////////////////////////////////////////////////////////////


    bool Window::has_mesh()
    {
        return !m_mesh_objects.empty();
    }

    bool Window::is_ready()
    {
        return m_initialized;
    }


    bool Window::is_closed()
    {
        return !m_window_open;
    }

}