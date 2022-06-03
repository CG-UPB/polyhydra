
#include "Window.h"

#include "input/Input.h"
#include "panels/LogWindow.h"
#include "panels/MeshView.h"
#include "panels/CustomUIPanel.h"
#include "panels/NewFileDialog.h"
#include "rendering/passes/ShapePass.h"

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

        // Mesh View
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
        std::shared_ptr<MeshObject> mesh = get_mesh_obj(mesh_id);

        // Unselect all elements
        if (mesh != nullptr)
            mesh->unselect_all();
        rendering_mutex.unlock();
    }

    void Window::unselect_all_elements()
    {
        rendering_mutex.lock();
        // Unselect all elements of all meshes
        for (const auto& [id, element] : m_mesh_objects)
        {
            element->unselect_all();
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

    void Window::set_face_color(int mesh_id, int ovm_face_id, const Color& color)
    {
        get_mesh_obj(mesh_id)->set_face_color(ovm_face_id, color);
    }

    void Window::set_cell_color(int mesh_id, int ovm_cell_id, const Color& color)
    {
        get_mesh_obj(mesh_id)->get_mvb()->set_cell_color(ovm_cell_id, color.r, color.g, color.b, color.a);
    }

    Color Window::get_face_color(int mesh_id, int ovm_face_id)
    {
        rendering_mutex.lock();
        auto ovm_face_handle = OpenVolumeMesh::FaceHandle(ovm_face_id);
        int ovm_halfface_id = get_mesh_obj(mesh_id)->get_ovm()->face_halffaces(ovm_face_handle)[0].idx();
        auto col = get_mesh_obj(mesh_id)->get_mvb()->get_halfface_color(ovm_halfface_id);
        Color color = Color(col.r, col.g, col.b, col.a);
        rendering_mutex.unlock();
        return color;
    }

    Color Window::get_cell_color(int mesh_id, int ovm_cell_id)
    {
        rendering_mutex.lock();
        auto col = get_mesh_obj(mesh_id)->get_mvb()->get_cell_color(ovm_cell_id);
        Color color = Color(col.r, col.g, col.b, col.a);
        rendering_mutex.unlock();
        return color;
    }

    void Window::set_mesh_color(Color color)
    {
        set_mesh_color(0, color);
    }

    void Window::set_mesh_color(int mesh_id, const Color& color)
    {
        // Change MeshObject Data
        get_mesh_obj(mesh_id)->get_data().color = color;
    }

    Color Window::get_mesh_color(int mesh_id)
    {
        // Get MeshObject Data
        return get_mesh_obj(mesh_id)->get_data().color;
    }


    void Window::set_mesh_selection_color(int mesh_id, const Color& color)
    {
        // Change MeshObject Data
        get_mesh_obj(mesh_id)->get_data().selection_color = color;
    }

    Color Window::get_mesh_selection_color(int mesh_id)
    {
        // Get MeshObject Data
        return get_mesh_obj(mesh_id)->get_data().selection_color;
    }

    void Window::set_mesh_visibility(bool visible)
    {
        // Change MeshObject Data
        set_mesh_visibility(0, visible);
    }

    void Window::set_mesh_visibility(int mesh_id, bool visible)
    {
        // Change MeshObject Data
        get_mesh_obj(mesh_id)->get_data().visible = visible;
    }

    bool Window::get_mesh_visibility(int mesh_id)
    {
        // Get MeshObject Data
        return get_mesh_obj(mesh_id)->get_data().visible;
    }

    void Window::set_mesh_slice_level(float slice_level)
    {
        set_mesh_slice_level(0, slice_level);
    }

    void Window::set_mesh_slice_level(int mesh_id, float slice_level)
    {
        // Change MeshObject Data
            get_mesh_obj(mesh_id)->get_data().slice_level = slice_level;
    }

    float Window::get_mesh_slice_level(int mesh_id)
    {
        // Get MeshObject Data
        return get_mesh_obj(mesh_id)->get_data().slice_level;
    }

    void Window::set_mesh_slice_locked(int mesh_id, bool locked)
    {
        // Change MeshObject Data
            get_mesh_obj(mesh_id)->get_data().slice_locked = locked;
    }

    bool Window::get_mesh_slice_locked(int mesh_id)
    {
        // Get MeshObject Data
        return get_mesh_obj(mesh_id)->get_data().slice_locked;
    }

    void Window::set_mesh_peel_level(float peel_level)
    {
        set_mesh_peel_level(0, peel_level);
    }

    void Window::set_mesh_peel_level(int mesh_id, float peel_level)
    {
        // Change MeshObject Data
        get_mesh_obj(mesh_id)->get_data().peel_level = peel_level;
    }

    float Window::get_mesh_peel_level(int mesh_id)
    {
        // Get MeshObject Data
        return get_mesh_obj(mesh_id)->get_data().peel_level;
    }

    void Window::set_mesh_cell_size(float cell_size)
    {
        set_mesh_cell_size(0, cell_size);
    }

    void Window::set_mesh_cell_size(int mesh_id, float cell_size)
    {
        // Change MeshObject Data
        get_mesh_obj(mesh_id)->get_data().cell_size = cell_size;
    }

    float Window::get_mesh_cell_size(int mesh_id)
    {
        // Get MeshObject Data
        return get_mesh_obj(mesh_id)->get_data().cell_size;
    }


    void Window::set_mesh_specular_exponent(int mesh_id, float exp)
    {
        get_mesh_obj(mesh_id)->get_data().specular_exponent = exp;
    }

    float Window::get_mesh_specular_exponent(int mesh_id)
    {
        return get_mesh_obj(mesh_id)->get_data().specular_exponent;
    }

    void Window::set_mesh_specular_strength(int mesh_id, float strength)
    {
        get_mesh_obj(mesh_id)->get_data().specular_strength = strength;
    }

    float Window::get_mesh_specular_strength(int mesh_id)
    {
        return get_mesh_obj(mesh_id)->get_data().specular_strength;
    }

    void Window::set_mesh_ambient_strength(int mesh_id, float strength)
    {
        get_mesh_obj(mesh_id)->get_data().ambient_strength = strength;
    }

    float Window::get_mesh_ambient_strength(int mesh_id)
    {
        return get_mesh_obj(mesh_id)->get_data().ambient_strength;
    }

    void Window::set_mesh_diffuse_strength(int mesh_id, float strength)
    {
        get_mesh_obj(mesh_id)->get_data().diffuse_strength = strength;
    }

    float Window::get_mesh_diffuse_strength(int mesh_id)
    {
        return get_mesh_obj(mesh_id)->get_data().diffuse_strength;
    }

    void Window::set_mesh_position(int mesh_id, float x, float y, float z)
    {
        // Change MeshObject Data
        get_mesh_obj(mesh_id)->get_data().position = glm::vec3(x, y, z);
    }

    void Window::set_mesh_scale(int mesh_id, float scale)
    {
        // Change MeshObject Data
        get_mesh_obj(mesh_id)->get_data().scale = glm::vec3(scale, scale, scale);
    }

    void Window::set_mesh_rounding_size(int mesh_id, float r_size)
    {
        // Change MeshObject Data
        get_mesh_obj(mesh_id)->get_data().rounding_size = r_size;
    }

    void Window::set_mesh_rounding_activated(int mesh_id, bool r_active)
    {
        // Change MeshObject Data
        get_mesh_obj(mesh_id)->get_data().rounding_active = r_active;
    }


    float Window::get_mesh_rounding_size(int mesh_id)
    {
        return get_mesh_obj(mesh_id)->get_data().rounding_size;
    }


    bool Window::get_mesh_rounding_activated(int mesh_id)
    {
        return get_mesh_obj(mesh_id)->get_data().rounding_active;
    }

    void Window::set_custom_imgui(void_callback vc)
    {
        rendering_mutex.lock();
        // Set Temporary Function
        m_temporary_new_custom_ui_function = std::move(vc);
        m_new_custom_ui_function_set = true;
        rendering_mutex.unlock();
    }

    void Window::set_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* mesh, int mesh_id)
    {
        rendering_mutex.lock();
        // Create MeshObject
        auto new_mesh_obj = std::make_shared<MeshObject>(mesh_id);
        new_mesh_obj->set_mesh_name(std::to_string(mesh->n_vertices()));
        new_mesh_obj->set_mesh(mesh);

        // Check if mesh_id of mesh already exist: yes -> replace it, no -> just insert it
        auto search = m_mesh_objects.find(mesh_id);
        if (search != m_mesh_objects.end())
        {
            search->second = std::move(new_mesh_obj);
        }
        else
        {
            // Insert Mesh
            m_mesh_objects.emplace(mesh_id, std::move(new_mesh_obj));
        }

        // If no other Mesh exists, focus the newly added Mesh
        if (m_mesh_objects.size() == 1) {
            rendering_mutex.unlock();
            set_mesh_focus(mesh_id);
            rendering_mutex.lock();
        }
        // Calculate Offsets
        int offset = 0;
        for (const auto& [id, mesh_obj] : m_mesh_objects)
        {
            mesh_obj->set_selection_offset(offset);
            offset = std::get<1>(mesh_obj->selection_offset()) + 1;
        }
        rendering_mutex.unlock();
    }

    int Window::add_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* mesh)
    {
        rendering_mutex.lock();
        // Generate Index
        int mesh_id = m_total_number_of_loaded_meshes++;

        // Create MeshObject
        auto new_mesh_obj = std::make_shared<MeshObject>(mesh_id);
        new_mesh_obj->set_mesh_name(std::to_string(mesh->n_vertices()));
        new_mesh_obj->set_mesh(mesh);

        // Add mesh to our map
        m_mesh_objects.emplace(mesh_id, std::move(new_mesh_obj));

        // If no other Mesh exists, focus the newly added Mesh
        if (m_mesh_objects.size() == 1) {

            rendering_mutex.unlock();
            set_mesh_focus(mesh_id);
            rendering_mutex.lock();
        }

        // Calculate Offsets
        int offset = 0;
        for (const auto& [id, mesh_obj] : m_mesh_objects)
        {
            mesh_obj->set_selection_offset(offset);
            offset = std::get<1>(mesh_obj->selection_offset()) + 1;
        }
        rendering_mutex.unlock();
        return mesh_id;
    }

    void Window::remove_mesh(int index)
    {
        rendering_mutex.lock();
        // Get MeshObject
        auto mesh_obj = get_mesh_obj(index);

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
            for (const auto& [id, obj] : m_mesh_objects)
            {
                new_active_mesh = id;
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

        rendering_mutex.unlock();
    }

    void Window::remove_all_meshes(){
        rendering_mutex.lock();

        std::vector<int> all_ids;

        // Iterate all Meshes to get_rgb their IDs
        for(const auto& [id, obj] : m_mesh_objects){
            all_ids.push_back(id);
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

        // Get MeshObject
        auto search = m_mesh_objects.find(index);
        if (search != m_mesh_objects.end())
        {
            m_focused_mesh = index;
        }
        // Focus Camera
        auto mesh_obj = get_mesh_obj(m_focused_mesh);
        if (mesh_obj != nullptr)
        {
            //m_renderer->set_zoom_point(mesh_obj->get_mesh_offset());
        }
    }

    int Window::get_mesh_focus() const
    {
        return m_focused_mesh;
    }

    std::shared_ptr<MeshObject> Window::get_mesh_obj(int index)
    {
        return m_mesh_objects.find(index) != m_mesh_objects.end() ? m_mesh_objects[index] : nullptr;
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

    void Window::camera_set_position(float x, float y, float z)
    {
        rendering_mutex.lock();
        m_mesh_view->m_render_data.camera.position = glm::vec3(x,y,z);
        rendering_mutex.unlock();
    }

    void Window::camera_focus_on(int mesh_id, int ovm_face_id, float time)
    {
        rendering_mutex.lock();
        // Get Face Normal
        auto face_normal = Window::instance().get_mesh_obj(mesh_id)->get_mvb()->get_face_normal(ovm_face_id);
        auto face_barycenter = Window::instance().get_mesh_obj(mesh_id)->get_mvb()->get_halfface_barycenter(ovm_face_id);
        // Focus
        //m_mesh_view->m_render_data.camera.focus_spot(face_barycenter, face_normal, time);
        rendering_mutex.unlock();
    }

    void Window::camera_focus_on(float target_x, float target_y, float target_z, float pos_x, float pos_y,
                                 float pos_z, float time)
    {
        rendering_mutex.lock();
        //m_mesh_view->m_render_data.camera.focus_spot({target_x,target_y,target_z}, {pos_x,pos_y,pos_z}, time);
        rendering_mutex.unlock();
    }

    void Window::camera_mode(Mode mode)
    {
        rendering_mutex.lock();

        m_mesh_view->m_render_data.camera.set_mode(mode);
        rendering_mutex.unlock();
    }

    void Window::camera_set_orbital_target(float x, float y, float z, float radius)
    {
        rendering_mutex.lock();

//        m_mesh_view->m_render_data.camera.m_orbital_origin = {x,y,z};
//        if(radius >= 1)
//            m_mesh_view->m_render_data.camera.radius = radius;
        rendering_mutex.unlock();
    }

    void Window::camera_look_at(float x, float y, float z)
    {
        rendering_mutex.lock();
        m_mesh_view->m_render_data.camera.look_at({x,y,z});
        rendering_mutex.unlock();
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


    bool Window::has_mesh()
    {
        return !m_mesh_objects.empty();
    }

    bool Window::is_ready() const
    {
        return m_initialized;
    }


    bool Window::is_closed() const
    {
        return !m_window_open;
    }

}