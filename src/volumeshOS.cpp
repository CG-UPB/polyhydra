
#define VOS_IGNORE_INVALID_MESH
#ifndef VOS_IGNORE_INVALID_MESH
    #define VOS_ASSERT_VALID_MESH(mesh, list) assert(list->get_mesh(mesh.get_id()) && "Invalid VMesh handle")
#else
    #define VOS_ASSERT_VALID_MESH(mesh, list) if (!list->get_mesh(mesh.get_id())) return
#endif

#include "vospch.h"

#include "volumeshOS.h"

#include "Window.h"
#include "mesh/MeshList.h"
#include "panels/NewFileDialog.h"
#include "settings/AppState.h"

#include <ctime>

namespace volumeshOS
{
    static std::vector<std::function<void()>> commands     = {};
    static std::unique_ptr<Internal::Window> window        = nullptr;
    static std::shared_ptr<Internal::MeshList> mesh_list   = nullptr;
    static std::shared_ptr<Internal::Camera> camera        = nullptr;
    static std::shared_ptr<Internal::ShapeRenderer> shapes = nullptr;
    static int current_mesh_id                             = 0;
    static int current_shape_id                            = 0;

    void initialize()
    {
        window = std::make_unique<Internal::Window>(1280, 720, "volumeshOS");
        window->initialize();
        mesh_list = window->panels.mesh_view->renderer->mesh_list;
        camera = window->panels.mesh_view->renderer->camera;
        shapes = window->panels.mesh_view->renderer->shapes;
    }

    void clean_up()
    {
        shapes = nullptr;
        camera = nullptr;
        mesh_list = nullptr;
        window->clean_up();
        window = nullptr;
    }

    int next_mesh_id()
    {
        return current_mesh_id++;
    }

    int next_shape_id()
    {
        return current_shape_id++;
    }

    void execute_commands()
    {
        for (auto& command : commands)
        {
            command();
        }
        commands.clear();
    }

    void open()
    {
        // initialize references and data
        initialize();

        // render loop
        while (!window->should_close())
        {
            execute_commands();
            window->render();
        }

        // clean up resources
        clean_up();
    }
    
    void close()
    {
        window->close();
    }


    void on_gui_render(const std::function<void()>& callback)
    {
        Internal::AppState::callbacks.on_gui_render = callback;
    }

    void on_cell_hover(const std::function<void(const VMesh, OpenVolumeMesh::CellHandle)>& callback)
    {
        Internal::AppState::callbacks.on_cell_hover = callback;
    }

    void on_face_hover(const std::function<void(const VMesh, OpenVolumeMesh::FaceHandle)>& callback)
    {
        Internal::AppState::callbacks.on_face_hover = callback;
    }

    void on_halfface_hover(const std::function<void(const VMesh, OpenVolumeMesh::HalfFaceHandle)>& callback)
    {
        Internal::AppState::callbacks.on_halfface_hover = callback;
    }

    void on_edge_hover(const std::function<void(const VMesh, OpenVolumeMesh::EdgeHandle)>& callback)
    {
        Internal::AppState::callbacks.on_edge_hover = callback;
    }

    void on_vertex_hover(const std::function<void(const VMesh, OpenVolumeMesh::VertexHandle)>& callback)
    {
        Internal::AppState::callbacks.on_vertex_hover = callback;
    }

    void on_cell_select(const std::function<void(const VMesh, OpenVolumeMesh::CellHandle)>& callback)
    {
        Internal::AppState::callbacks.on_cell_select = callback;
    }

    void on_face_select(const std::function<void(const VMesh, OpenVolumeMesh::FaceHandle)>& callback)
    {
        Internal::AppState::callbacks.on_face_select = callback;
    }

    void on_halfface_select(const std::function<void(const VMesh, OpenVolumeMesh::HalfFaceHandle)>& callback)
    {
        Internal::AppState::callbacks.on_halfface_select = callback;
    }

    void on_edge_select(const std::function<void(const VMesh, OpenVolumeMesh::EdgeHandle)>& callback)
    {
        Internal::AppState::callbacks.on_edge_select = callback;
    }

    void on_vertex_select(const std::function<void(const VMesh, OpenVolumeMesh::VertexHandle)>& callback)
    {
        Internal::AppState::callbacks.on_vertex_select = callback;
    }

    void on_position_select(const std::function<void(float, float, float)>& callback)
    {
        Internal::AppState::callbacks.on_position_select = callback;
    }

    VMesh load(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* instance, const char* name)
    {
        int id = next_mesh_id();
        VMesh vmesh(id);
        commands.emplace_back([id, instance, name]{
            mesh_list->add_mesh(id, instance);
            if (name != nullptr)
            {
                mesh_list->set_name(id, name);
            }
        });
        focus_camera(VMesh(id));
        return vmesh;
    }

    VMesh load(const std::string& path, const char* name)
    {
        int id = next_mesh_id();
        VMesh vmesh(id);
        commands.emplace_back([id, path, name]{
            mesh_list->add_mesh(id, path);
            if (name != nullptr)
            {
                mesh_list->set_name(id, name);
            }
            else
            {
                FS_NAMESPACE::path file_path(path);
                auto file_name = file_path.stem().filename().string();
                mesh_list->set_name(id, file_name);
            }
        });
        focus_camera(VMesh(id));
        return vmesh;
    }

    VMesh load(const char* path, const char* name)
    {
        int id = next_mesh_id();
        VMesh vmesh(id);
        commands.emplace_back([id, path, name]{
            mesh_list->add_mesh(id, path);
            if (name != nullptr)
            {
                mesh_list->set_name(id, name);
            }
            else
            {
                FS_NAMESPACE::path file_path(path);
                auto file_name = file_path.stem().filename().string();
                mesh_list->set_name(id, file_name);
            }
        });
        focus_camera(VMesh(id));
        return vmesh;
    }

    VMesh load_from_dialog(const std::string& title, const char* name)
    {
        if (auto file = volumeshOS::file_dialog(title))
        {
            return volumeshOS::load(file, name);
        }
        return VMesh(-1);
    }

    void update(const VMesh& mesh, OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* instance)
    {
        commands.emplace_back([mesh, instance]{
            mesh_list->set_mesh(mesh.get_id(), instance);
        });
    }

    void update(const VMesh& mesh, const std::string& path)
    {
        commands.emplace_back([mesh, path]{
            mesh_list->set_mesh(mesh.get_id(), path);
        });
    }

    void update(const VMesh& mesh, const char* path)
    {
        commands.emplace_back([mesh, path]{
            mesh_list->set_mesh(mesh.get_id(), path);
        });
    }

    void clear(const VMesh& mesh)
    {
        commands.emplace_back([mesh]{
            mesh_list->delete_mesh(mesh.get_id());
        });
    }

    void clear()
    {
        commands.emplace_back([]{
            mesh_list->delete_meshes();
        });
    }

    std::vector<VMesh> get_meshes()
    {
        std::vector<VMesh> meshes;
        mesh_list->iterate([&meshes](auto id, auto mesh){
            meshes.push_back(VMesh(id));
        });
        return meshes;
    }

    VMesh get_focused_mesh()
    {
        return VMesh(mesh_list->get_focused_mesh_id());
    }

    bool is_valid(const VMesh& mesh)
    {
        return mesh.get_id() >= 0 && mesh_list->get_mesh(mesh.get_id()) != nullptr;
    }

    [[nodiscard]] OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* get_ovm(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_mesh(mesh.get_id())->get_ovm().get();
    }

    void load_configuration(const VMesh& mesh, const std::string& path)
    {

    }

    void save_configuration(const VMesh& mesh, const std::string& path)
    {

    }

    template<typename Vec4T>
    void set_color(const Vec4T& color)
    {
        commands.emplace_back([color]{
            auto col = Internal::to_glm_vec4(color);
            mesh_list->set_color(col);
        });
    }

    template<typename Vec4T>
    void set_color(const VMesh& mesh, const Vec4T& color)
    {
        commands.emplace_back([mesh, color]{
            auto col = Internal::to_glm_vec4(color);
            mesh_list->set_color(mesh.get_id(), col);
        });
    }

    template<typename Vec4T>
    void set_color(const VMesh& mesh, OpenVolumeMesh::CellHandle cell, const Vec4T& color)
    {
        commands.emplace_back([mesh, cell, color]{
            auto col = Internal::to_glm_vec4(color);
            mesh_list->set_color(mesh.get_id(), cell, col);
        });
    }

    template<typename Vec4T>
    void set_color(const VMesh& mesh, OpenVolumeMesh::FaceHandle face, const Vec4T& color)
    {
        commands.emplace_back([mesh, face, color]{
            auto col = Internal::to_glm_vec4(color);
            mesh_list->set_color(mesh.get_id(), face, col);
        });
    }

    template<typename Vec4T>
    void set_color(const VMesh& mesh, OpenVolumeMesh::HalfFaceHandle halfface, const Vec4T& color)
    {
        commands.emplace_back([mesh, halfface, color]{
            auto col = Internal::to_glm_vec4(color);
            mesh_list->set_color(mesh.get_id(), halfface, col);
        });
    }

    template<typename Vec4T>
    void set_color(const VMesh& mesh, OpenVolumeMesh::EdgeHandle edge, const Vec4T& color)
    {
//        commands.emplace_back([mesh, edge, color]{
//            mesh_list->set_color(mesh.get_id(), edge, color);
//        });
    }

    template<typename Vec4T>
    void set_color(const VMesh& mesh, OpenVolumeMesh::VertexHandle vertex, const Vec4T& color)
    {
//        commands.emplace_back([mesh, vertex, color]{
//            mesh_list->set_color(mesh.get_id(), vertex, color);
//        });
    }

    void set_name(const VMesh& mesh, const std::string& name)
    {
        commands.emplace_back([mesh, name]{
            mesh_list->set_name(mesh.get_id(), name);
        });
    }

    void select(const VMesh& mesh, OpenVolumeMesh::VertexHandle vertex)
    {
        commands.emplace_back([mesh, vertex]{
            mesh_list->select(EntityType::Vertex, mesh.get_id(), vertex.idx());
        });
    }

    void select(const VMesh& mesh, OpenVolumeMesh::EdgeHandle edge)
    {
        commands.emplace_back([mesh, edge]{
            mesh_list->select(EntityType::Edge, mesh.get_id(), edge.idx());
        });
    }

    void select(const VMesh& mesh, OpenVolumeMesh::HalfFaceHandle halfface)
    {
        commands.emplace_back([mesh, halfface]{
            mesh_list->select(EntityType::Halfface, mesh.get_id(), halfface.idx());
        });
    }

    void select(const VMesh& mesh, OpenVolumeMesh::FaceHandle face)
    {
        commands.emplace_back([mesh, face]{
            mesh_list->select(EntityType::Face, mesh.get_id(), face.idx());
        });
    }

    void select(const VMesh& mesh, OpenVolumeMesh::CellHandle cell)
    {
        commands.emplace_back([mesh, cell]{
            mesh_list->select(EntityType::Cell, mesh.get_id(), cell.idx());
        });
    }

    void deselect(const VMesh& mesh, OpenVolumeMesh::VertexHandle vertex)
    {
        commands.emplace_back([mesh, vertex]{
            mesh_list->deselect(EntityType::Vertex, mesh.get_id(), vertex.idx());
        });
    }

    void deselect(const VMesh& mesh, OpenVolumeMesh::EdgeHandle edge)
    {
        commands.emplace_back([mesh, edge]{
            mesh_list->deselect(EntityType::Edge, mesh.get_id(), edge.idx());
        });
    }

    void deselect(const VMesh& mesh, OpenVolumeMesh::HalfFaceHandle halfface)
    {
        commands.emplace_back([mesh, halfface]{
            mesh_list->deselect(EntityType::Halfface, mesh.get_id(), halfface.idx());
        });
    }

    void deselect(const VMesh& mesh, OpenVolumeMesh::FaceHandle face)
    {
        commands.emplace_back([mesh, face]{
            mesh_list->deselect(EntityType::Face, mesh.get_id(), face.idx());
        });
    }

    void deselect(const VMesh& mesh, OpenVolumeMesh::CellHandle cell)
    {
        commands.emplace_back([mesh, cell]{
            mesh_list->deselect(EntityType::Cell, mesh.get_id(), cell.idx());
        });
    }

    void set_ambient(const VMesh& mesh, float ambient)
    {
        commands.emplace_back([mesh, ambient]{
            mesh_list->set_ambient(mesh.get_id(), ambient);
        });
    }

    void set_diffuse(const VMesh& mesh, float diffuse)
    {
        commands.emplace_back([mesh, diffuse]{
            mesh_list->set_diffuse(mesh.get_id(), diffuse);
        });
    }

    void set_specular(const VMesh& mesh, float specular)
    {
        commands.emplace_back([mesh, specular]{
            mesh_list->set_specular(mesh.get_id(), specular);
        });
    }

    void set_specular_coefficient(const VMesh& mesh, float coefficient)
    {
        commands.emplace_back([mesh, coefficient]{
            mesh_list->set_specular_coefficient(mesh.get_id(), coefficient);
        });
    }

    void set_phong(const VMesh& mesh, float ambient, float diffuse, float specular, float coefficient)
    {
        commands.emplace_back([mesh, ambient, diffuse, specular, coefficient]{
            mesh_list->set_phong(mesh.get_id(), ambient, diffuse, specular, coefficient);
        });
    }

    void set_position(const VMesh& mesh, float x, float y, float z)
    {
        commands.emplace_back([mesh, x, y, z]{
            mesh_list->set_position(mesh.get_id(), x, y, z);
        });
    }

    template<typename Vec3T>
    void set_position(const VMesh& mesh, const Vec3T& position)
    {
        commands.emplace_back([mesh, position]{
            auto pos = Internal::to_glm_vec3(position);
            mesh_list->set_position(mesh.get_id(), pos.x, pos.y, pos.z);
        });
    }

    void set_scale(const VMesh& mesh, float scale)
    {
        commands.emplace_back([mesh, scale]{
            mesh_list->set_scale(mesh.get_id(), scale);
        });
    }

    void set_rotation(const VMesh& mesh, float x, float y, float z)
    {
        commands.emplace_back([mesh, x, y, z]{
            mesh_list->set_rotation(mesh.get_id(), x, y, z);
        });
    }

    template<typename Vec3T>
    void set_rotation(const VMesh& mesh, const Vec3T& rotation)
    {
        commands.emplace_back([mesh, rotation]{
            auto rot = Internal::to_glm_vec3(rotation);
            mesh_list->set_rotation(mesh.get_id(), rot.x, rot.y, rot.z);
        });
    }

    void reset_rotation(const VMesh& mesh)
    {
        commands.emplace_back([mesh]{
            mesh_list->reset_rotation(mesh.get_id());
        });
    }

    void set_slice_factor(const VMesh& mesh, const float level)
    {
        commands.emplace_back([mesh, level]{
            mesh_list->set_slice_factor(mesh.get_id(), level);
        });
    }

    void set_slice_lock(const VMesh& mesh, const bool locked)
    {
        commands.emplace_back([mesh, locked]{
            mesh_list->set_slice_lock(mesh.get_id(), locked);
        });
    }

    void set_peel_level(const VMesh& mesh, const float level)
    {
        commands.emplace_back([mesh, level]{
            mesh_list->set_peel_level(mesh.get_id(), level);
        });
    }

    void set_cell_rounding(const VMesh& mesh, float rounding)
    {
        commands.emplace_back([mesh, rounding]{
            mesh_list->set_cell_rounding(mesh.get_id(), rounding);
        });
    }

    void activate_rounding(const VMesh& mesh, float rounding)
    {
        commands.emplace_back([mesh, rounding]{
            mesh_list->activate_rounding(mesh.get_id(), rounding);
        });
    }

    void activate_rounding(const VMesh& mesh, bool rounding)
    {
        commands.emplace_back([mesh, rounding]{
            mesh_list->activate_rounding(mesh.get_id(), rounding);
        });
    }

    void set_cell_size(const VMesh& mesh, const float size)
    {
        commands.emplace_back([mesh, size]{
            mesh_list->set_cell_size(mesh.get_id(), size);
        });
    }

    void set_visibility(const VMesh& mesh, OpenVolumeMesh::CellHandle cell, bool visible)
    {
        commands.emplace_back([mesh, cell, visible](){
            mesh_list->set_visibility(mesh.get_id(), cell, visible);
        });
    }

    void set_focused_mesh(VMesh mesh)
    {
        commands.emplace_back([mesh](){
            mesh_list->set_focused_mesh(mesh.get_id());
        });
    }

    void get_focused_mesh(const VMesh& mesh)
    {
        commands.emplace_back([mesh](){
            mesh_list->set_focused_mesh(mesh.get_id());
        });
    }

    void set_visibility(const VMesh& mesh, const bool visible)
    {
        commands.emplace_back([mesh, visible]{
            mesh_list->set_visibility(mesh.get_id(), visible);
        });
    }

    void reset_visibility(const VMesh& mesh)
    {
        commands.emplace_back([mesh]{
            mesh_list->reset_visibility(mesh.get_id());
            shapes->reset_visibility();
        });
    }

    void isolate(const VMesh& mesh, OpenVolumeMesh::CellHandle cell)
    {
        commands.emplace_back([mesh, cell]{
            mesh_list->isolate(mesh.get_id(), cell);
        });
    }

    void hide(const VMesh& mesh, OpenVolumeMesh::CellHandle cell)
    {
        commands.emplace_back([mesh, cell]{
            mesh_list->hide(mesh.get_id(), cell);
        });
    }


    void set_camera_position(float x, float y, float z)
    {
        commands.emplace_back([x, y, z]{
            camera->set_position(glm::vec3(x, y, z));
        });
    }

    template<typename Vec3T>
    void set_camera_position(const Vec3T& position)
    {
        commands.emplace_back([position]{
            auto pos = Internal::to_glm_vec3(position);
            camera->set_position(pos);
        });
    }

    void set_camera_view_direction(float x, float y, float z)
    {
        commands.emplace_back([x, y, z]{
            camera->set_view_direction(glm::vec3(x, y, z));
        });
    }

    template<typename Vec3T>
    void set_camera_view_direction(const Vec3T& direction)
    {
        commands.emplace_back([direction]{
            auto dir = Internal::to_glm_vec3(direction);
            camera->set_view_direction(dir);
        });
    }

    void set_camera_mode(Internal::CameraMode mode)
    {
        commands.emplace_back([mode]{
            camera->set_mode(mode);
        });
    }

    void focus_camera(const VMesh& mesh)
    {
        commands.emplace_back([mesh]{
            if(auto mesh_obj = mesh_list->get_mesh(mesh.get_id()))
            {
                auto pos = mesh_obj->get_data().position;
                camera->animated_look_at(pos);
            }
        });
    }

    template<typename Vec3T>
    void set_background_color(const Vec3T& color)
    {
        commands.emplace_back([color]{
            auto col = Internal::to_glm_vec3(color);
            Internal::AppState::settings.general.background_color = col;
        });
    }

    void export_image(const ExportOptions& options)
    {
        commands.emplace_back([options]{
            auto& renderer = window->panels.mesh_view->renderer;
            auto now = time(nullptr);
            char buffer[30];
            strftime(buffer, 30, "%F%H-%M-%S.png", localtime(&now));
            renderer->export_image(std::string(buffer), options);
        });
    }

    void export_image(const std::string& path, const ExportOptions& options)
    {
        commands.emplace_back([path, options]{
            auto& renderer = window->panels.mesh_view->renderer;
            renderer->export_image(path, options);
        });
    }

    void log(const std::string& message)
    {
        commands.emplace_back([message]{
            window->panels.mesh_view->log_window->add_message(message, Internal::Info);
            volumeshOS::Internal::Log::info(message);
        });
    }

    void warn(const std::string& message)
    {
        commands.emplace_back([message]{
            window->panels.mesh_view->log_window->add_message(message, Internal::Warning);
            volumeshOS::Internal::Log::warn(message);
        });
    }

    void error(const std::string& message)
    {
        commands.emplace_back([message]{
            window->panels.mesh_view->log_window->add_message(message, Internal::Error);
            volumeshOS::Internal::Log::error(message);
        });
    }

    void clear_logs()
    {
        commands.emplace_back([]{
            window->panels.mesh_view->log_window->clear_logs();
        });
    }

    void hide_log_window(bool hide)
    {
        commands.emplace_back([hide]{
            window->panels.mesh_view->log_window->hide_log_window(hide);
        });
    }

    int get_viewport_width()
    {
        return window->panels.mesh_view->renderer->frame.width;
    }

    int get_viewport_height()
    {
        return window->panels.mesh_view->renderer->frame.height;
    }

    float get_ambient(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_ambient(mesh.get_id());
    }

    float get_diffuse(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_diffuse(mesh.get_id());
    }

    float get_specular(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_specular(mesh.get_id());
    }

    float get_specular_coefficient(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_specular_coefficient(mesh.get_id());
    }

    template<typename Vec3T>
    Vec3T get_position(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return Internal::glm_vec3_to<Vec3T>(mesh_list->get_position(mesh.get_id()));
    }

    float get_scale(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_scale(mesh.get_id());
    }

    template<typename Vec3T>
    Vec3T get_rotation(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return Internal::glm_vec3_to<Vec3T>(mesh_list->get_rotation(mesh.get_id()));
    }

    float get_slice_factor(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_slice_factor(mesh.get_id());
    }

    bool get_slice_lock(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_slice_lock(mesh.get_id());
    }

    float get_peel_level(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_peel_level(mesh.get_id());
    }

    int get_max_peel_depth(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_max_peel_depth(mesh.get_id());
    }

    float get_cell_rounding(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_cell_rounding(mesh.get_id());
    }

    float get_cell_size(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_cell_size(mesh.get_id());
    }

    bool get_visibility(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_visibility(mesh.get_id());
    }

    bool get_visibility(const VMesh& mesh, OpenVolumeMesh::CellHandle cell)
    {
        assert(mesh.is_valid());
        return mesh_list->get_visibility(mesh.get_id(), cell);
    }

    const char* file_dialog(const std::string& title)
    {
        Internal::NewFileDialog dialog;
        return dialog.open_dialog(title.c_str());
    }

    template<typename Vec4T>
    Vec4T get_color(const VMesh& mesh)
    {
        return Internal::glm_vec4_to<Vec4T>(mesh_list->get_color(mesh.get_id()));
    }

    template<typename Vec4T>
    Vec4T get_color(const VMesh& mesh, OpenVolumeMesh::CellHandle cell)
    {
        return Internal::glm_vec4_to<Vec4T>(mesh_list->get_color(mesh.get_id(), cell));
    }

    template<typename Vec4T>
    Vec4T get_color(const VMesh& mesh, OpenVolumeMesh::HalfFaceHandle halfface)
    {
        return Internal::glm_vec4_to<Vec4T>(mesh_list->get_color(mesh.get_id(), halfface));
    }

    template<typename Vec4T>
    Vec4T get_color(const VMesh& mesh, OpenVolumeMesh::EdgeHandle edge)
    {
        return Internal::glm_vec4_to<Vec4T>(mesh_list->get_color(mesh.get_id(), edge));
    }

    template<typename Vec4T>
    Vec4T get_color(const VMesh& mesh, OpenVolumeMesh::VertexHandle vertex)
    {
        return Internal::glm_vec4_to<Vec4T>(mesh_list->get_color(mesh.get_id(), vertex));
    }


    [[nodiscard]] const std::string& get_name(const VMesh& mesh)
    {
        return mesh_list->get_name(mesh.get_id());
    }

    template<typename Type>
    [[nodiscard]] std::unique_ptr<Internal::BaseShape> get_concrete_shape(int id)
    {
        if constexpr (std::is_same_v<Type, VBox>)
        {
            return std::make_unique<Internal::BoxShape>(id);
        }
        else if constexpr (std::is_same_v<Type, VCylinder>)
        {
            return std::make_unique<Internal::CylinderShape>(id);
        }
        else if constexpr (std::is_same_v<Type, VSphere>)
        {
            return std::make_unique<Internal::SphereShape>(id);
        }
        else if constexpr (std::is_same_v<Type, VCone>)
        {
            return std::make_unique<Internal::ConeShape>(id);
        }
        else if constexpr (std::is_same_v<Type, VArrow>)
        {
            return std::make_unique<Internal::ArrowShape>(id);
        }
        assert(false && "Invalid shape type");
    }

    template<typename Type>
    [[nodiscard]] Type get_shape_api(int id)
    {
        if constexpr (std::is_same_v<Type, VBox>)
        {
            return VBox(id);
        }
        else if constexpr (std::is_same_v<Type, VCylinder>)
        {
            return VCylinder(id);
        }
        else if constexpr (std::is_same_v<Type, VSphere>)
        {
            return VSphere(id);
        }
        else if constexpr (std::is_same_v<Type, VCone>)
        {
            return VCone(id);
        }
        else if constexpr (std::is_same_v<Type, VArrow>)
        {
            return VArrow(id);
        }
        assert(false && "Invalid shape type");
    }

    template<typename ShapeType>
    [[nodiscard]] ShapeType add_shape()
    {
        auto id = next_shape_id();
        commands.emplace_back([id]{
            auto shape = get_concrete_shape<ShapeType>(id);
            shapes->add_shape(std::move(shape));
        });
        return get_shape_api<ShapeType>(id);
    }

    template<typename ShapeType>
    [[nodiscard]] ShapeType add_shape(const VMesh& mesh)
    {
        auto id = next_shape_id();
        commands.emplace_back([id, mesh]{
            auto shape = get_concrete_shape<ShapeType>(id);
            shape->parent_mesh = mesh.get_id();
            shapes->add_shape(std::move(shape));
        });
        return get_shape_api<ShapeType>(id);
    }

    template<typename ShapeType>
    [[nodiscard]] ShapeType add_shape(const VMesh& mesh, OpenVolumeMesh::CellHandle cell)
    {
        auto id = next_shape_id();
        commands.emplace_back([id, mesh, cell]{
            auto shape = get_concrete_shape<ShapeType>(id);
            shape->parent_mesh = mesh.get_id();
            shape->cell = cell;
            shapes->add_shape(std::move(shape));
        });
        return get_shape_api<ShapeType>(id);
    }

    void remove_shape(const VShape& shape)
    {
        commands.emplace_back([shape]{
            shapes->remove_shape(shape.get_id());
        });
    }

    void remove_shapes()
    {
        commands.emplace_back([]{
            shapes->remove_all();
        });
    }

    void set_position(const VShape& shape, float x, float y, float z)
    {
        commands.emplace_back([shape, x, y, z]{
            shapes->set_position(shape.get_id(), x, y, z);
        });
    }

    template<typename Vec3T>
    void set_position(const VShape& shape, const Vec3T& position)
    {
        commands.emplace_back([shape, position]{
            auto pos = Internal::to_glm_vec3(position);
            shapes->set_position(shape.get_id(), pos.x, pos.y, pos.z);
        });
    }

    void set_direction(const VShape& shape, float axis_x, float axis_y, float axis_z, float angle)
    {
        commands.emplace_back([shape, axis_x, axis_y, axis_z, angle]{
            shapes->set_rotation(shape.get_id(), axis_x, axis_y, axis_z, angle);
        });
    }

    template<typename Vec3T>
    void set_direction(const VShape& shape, const Vec3T& axis, float angle)
    {
        commands.emplace_back([shape, axis, angle]{
            auto dir = Internal::to_glm_vec3(axis);
            shapes->set_rotation(shape.get_id(), dir.x, dir.y, dir.z, angle);
        });
    }

    void set_scale(const VShape& shape, float scalar)
    {
        commands.emplace_back([shape, scalar]{
            shapes->set_scale(shape.get_id(), scalar, scalar, scalar);
        });
    }

    void set_scale(const VShape& shape, float x, float y, float z)
    {
        commands.emplace_back([shape, x, y, z]{
            shapes->set_scale(shape.get_id(), x, y, z);
        });
    }

    template<typename Vec3T>
    void set_scale(const VShape& shape, const Vec3T& scale)
    {
        commands.emplace_back([shape, scale]{
            auto scl = Internal::to_glm_vec3(scale);
            shapes->set_scale(shape.get_id(), scl.x, scl.y, scl.z);
        });
    }

    template<typename Vec4T>
    void set_color(const VShape& shape, const Vec4T& color)
    {
        commands.emplace_back([shape, color]{
            auto col = Internal::to_glm_vec4(color);
            shapes->set_color(shape.get_id(), col.r, col.g, col.b);
        });
    }

    template<typename Vec3T>
    [[nodiscard]] Vec3T get_position(const VShape& shape)
    {
        return Internal::glm_vec3_to<Vec3T>(shapes->get_position(shape.get_id()));
    }

    template<typename Vec3T>
    [[nodiscard]] Vec3T get_scale(const VShape& shape)
    {
        return Internal::glm_vec3_to<Vec3T>(shapes->get_scale(shape.get_id()));
    }

    void set_tip_height(const VArrow& shape, float tip_height)
    {
        commands.emplace_back([shape, tip_height]{
            auto* arrow = shapes->get_shape_and_update_buffers<Internal::ArrowShape>(shape.get_id());
            arrow->tip_height_percentage = tip_height;
        });
    }

    void set_base_width(const VArrow& shape, float base_width)
    {
        commands.emplace_back([shape, base_width]{
            auto* arrow = shapes->get_shape_and_update_buffers<Internal::ArrowShape>(shape.get_id());
            arrow->base_width_percentage = base_width;
        });
    }

    template void set_color<glm::vec4>(const glm::vec4&);
    template void set_color<OpenVolumeMesh::Vec4d>(const OpenVolumeMesh::Vec4d&);
    template void set_color<OpenVolumeMesh::Vec4f>(const OpenVolumeMesh::Vec4f&);
    template void set_color<std::array<double, 4>>(const std::array<double, 4>&);
    template void set_color<std::array<float, 4>>(const std::array<float, 4>&);

    template void set_color<glm::vec4>(const VMesh&, const glm::vec4&);
    template void set_color<OpenVolumeMesh::Vec4d>(const VMesh&, const OpenVolumeMesh::Vec4d&);
    template void set_color<OpenVolumeMesh::Vec4f>(const VMesh&, const OpenVolumeMesh::Vec4f&);
    template void set_color<std::array<double, 4>>(const VMesh&, const std::array<double, 4>&);
    template void set_color<std::array<float, 4>>(const VMesh&, const std::array<float, 4>&);

    template void set_color<glm::vec4>(const VMesh&, OpenVolumeMesh::CellHandle, const glm::vec4&);
    template void set_color<OpenVolumeMesh::Vec4d>(const VMesh&, OpenVolumeMesh::CellHandle, const OpenVolumeMesh::Vec4d&);
    template void set_color<OpenVolumeMesh::Vec4f>(const VMesh&, OpenVolumeMesh::CellHandle, const OpenVolumeMesh::Vec4f&);
    template void set_color<std::array<double, 4>>(const VMesh&, OpenVolumeMesh::CellHandle, const std::array<double, 4>&);
    template void set_color<std::array<float, 4>>(const VMesh&, OpenVolumeMesh::CellHandle, const std::array<float, 4>&);

    template void set_color(const VMesh&, OpenVolumeMesh::FaceHandle, const glm::vec4&);
    template void set_color<OpenVolumeMesh::Vec4d>(const VMesh&, OpenVolumeMesh::FaceHandle, const OpenVolumeMesh::Vec4d&);
    template void set_color<OpenVolumeMesh::Vec4f>(const VMesh&, OpenVolumeMesh::FaceHandle, const OpenVolumeMesh::Vec4f&);
    template void set_color<std::array<double, 4>>(const VMesh&, OpenVolumeMesh::FaceHandle, const std::array<double, 4>&);
    template void set_color<std::array<float, 4>>(const VMesh&, OpenVolumeMesh::FaceHandle, const std::array<float, 4>&);

    template void set_color<glm::vec4>(const VMesh&, OpenVolumeMesh::HalfFaceHandle, const glm::vec4&);
    template void set_color<OpenVolumeMesh::Vec4d>(const VMesh&, OpenVolumeMesh::HalfFaceHandle, const OpenVolumeMesh::Vec4d&);
    template void set_color<OpenVolumeMesh::Vec4f>(const VMesh&, OpenVolumeMesh::HalfFaceHandle, const OpenVolumeMesh::Vec4f&);
    template void set_color<std::array<double, 4>>(const VMesh&, OpenVolumeMesh::HalfFaceHandle, const std::array<double, 4>&);
    template void set_color<std::array<float, 4>>(const VMesh&, OpenVolumeMesh::HalfFaceHandle, const std::array<float, 4>&);

    template void set_color<glm::vec4>(const VMesh&, OpenVolumeMesh::EdgeHandle, const glm::vec4&);
    template void set_color<OpenVolumeMesh::Vec4d>(const VMesh&, OpenVolumeMesh::EdgeHandle, const OpenVolumeMesh::Vec4d&);
    template void set_color<OpenVolumeMesh::Vec4f>(const VMesh&, OpenVolumeMesh::EdgeHandle, const OpenVolumeMesh::Vec4f&);
    template void set_color<std::array<double, 4>>(const VMesh&, OpenVolumeMesh::EdgeHandle, const std::array<double, 4>&);
    template void set_color<std::array<float, 4>>(const VMesh&, OpenVolumeMesh::EdgeHandle, const std::array<float, 4>&);

    template void set_color<glm::vec4>(const VMesh&, OpenVolumeMesh::VertexHandle, const glm::vec4&);
    template void set_color<OpenVolumeMesh::Vec4d>(const VMesh&, OpenVolumeMesh::VertexHandle, const OpenVolumeMesh::Vec4d&);
    template void set_color<OpenVolumeMesh::Vec4f>(const VMesh&, OpenVolumeMesh::VertexHandle, const OpenVolumeMesh::Vec4f&);
    template void set_color<std::array<double, 4>>(const VMesh&, OpenVolumeMesh::VertexHandle, const std::array<double, 4>&);
    template void set_color<std::array<float, 4>>(const VMesh&, OpenVolumeMesh::VertexHandle, const std::array<float, 4>&);


    template glm::vec4 get_color<glm::vec4>(const VMesh&);
    template OpenVolumeMesh::Vec4d get_color<OpenVolumeMesh::Vec4d>(const VMesh&);
    template OpenVolumeMesh::Vec4f get_color<OpenVolumeMesh::Vec4f>(const VMesh&);
    template std::array<double, 4> get_color<std::array<double, 4>>(const VMesh&);
    template std::array<float, 4> get_color<std::array<float, 4>>(const VMesh&);

    template glm::vec4 get_color<glm::vec4>(const VMesh&, OpenVolumeMesh::CellHandle);
    template OpenVolumeMesh::Vec4d get_color<OpenVolumeMesh::Vec4d>(const VMesh&, OpenVolumeMesh::CellHandle);
    template OpenVolumeMesh::Vec4f get_color<OpenVolumeMesh::Vec4f>(const VMesh&, OpenVolumeMesh::CellHandle);
    template std::array<double, 4> get_color<std::array<double, 4>>(const VMesh&, OpenVolumeMesh::CellHandle);
    template std::array<float, 4> get_color<std::array<float, 4>>(const VMesh&, OpenVolumeMesh::CellHandle);

    template glm::vec4 get_color<glm::vec4>(const VMesh&, OpenVolumeMesh::HalfFaceHandle);
    template OpenVolumeMesh::Vec4d get_color<OpenVolumeMesh::Vec4d>(const VMesh&, OpenVolumeMesh::HalfFaceHandle);
    template OpenVolumeMesh::Vec4f get_color<OpenVolumeMesh::Vec4f>(const VMesh&, OpenVolumeMesh::HalfFaceHandle);
    template std::array<double, 4> get_color<std::array<double, 4>>(const VMesh&, OpenVolumeMesh::HalfFaceHandle);
    template std::array<float, 4> get_color<std::array<float, 4>>(const VMesh&, OpenVolumeMesh::HalfFaceHandle);

    template glm::vec4 get_color<glm::vec4>(const VMesh&, OpenVolumeMesh::EdgeHandle);
    template OpenVolumeMesh::Vec4d get_color<OpenVolumeMesh::Vec4d>(const VMesh&, OpenVolumeMesh::EdgeHandle);
    template OpenVolumeMesh::Vec4f get_color<OpenVolumeMesh::Vec4f>(const VMesh&, OpenVolumeMesh::EdgeHandle);
    template std::array<double, 4> get_color<std::array<double, 4>>(const VMesh&, OpenVolumeMesh::EdgeHandle);
    template std::array<float, 4> get_color<std::array<float, 4>>(const VMesh&, OpenVolumeMesh::EdgeHandle);

    template glm::vec4 get_color<glm::vec4>(const VMesh&, OpenVolumeMesh::VertexHandle);
    template OpenVolumeMesh::Vec4d get_color<OpenVolumeMesh::Vec4d>(const VMesh&, OpenVolumeMesh::VertexHandle);
    template OpenVolumeMesh::Vec4f get_color<OpenVolumeMesh::Vec4f>(const VMesh&, OpenVolumeMesh::VertexHandle);
    template std::array<double, 4> get_color<std::array<double, 4>>(const VMesh&, OpenVolumeMesh::VertexHandle);
    template std::array<float, 4> get_color<std::array<float, 4>>(const VMesh&, OpenVolumeMesh::VertexHandle);


    template void set_position<glm::vec3>(const VMesh&, const glm::vec3&);
    template void set_position<OpenVolumeMesh::Vec3d>(const VMesh&, const OpenVolumeMesh::Vec3d&);
    template void set_position<OpenVolumeMesh::Vec3f>(const VMesh&, const OpenVolumeMesh::Vec3f&);
    template void set_position<std::array<double, 3>>(const VMesh&, const std::array<double, 3>&);
    template void set_position<std::array<float, 3>>(const VMesh&, const std::array<float, 3>&);

    template void set_rotation<glm::vec3>(const VMesh&, const glm::vec3&);
    template void set_rotation<OpenVolumeMesh::Vec3d>(const VMesh&, const OpenVolumeMesh::Vec3d&);
    template void set_rotation<OpenVolumeMesh::Vec3f>(const VMesh&, const OpenVolumeMesh::Vec3f&);
    template void set_rotation<std::array<double, 3>>(const VMesh&, const std::array<double, 3>&);
    template void set_rotation<std::array<float, 3>>(const VMesh&, const std::array<float, 3>&);

    template glm::vec3 get_position<glm::vec3>(const VMesh&);
    template OpenVolumeMesh::Vec3d get_position<OpenVolumeMesh::Vec3d>(const VMesh&);
    template OpenVolumeMesh::Vec3f get_position<OpenVolumeMesh::Vec3f>(const VMesh&);
    template std::array<double, 3> get_position<std::array<double, 3>>(const VMesh&);
    template std::array<float, 3> get_position<std::array<float, 3>>(const VMesh&);

    template glm::vec3 get_rotation<glm::vec3>(const VMesh&);
    template OpenVolumeMesh::Vec3d get_rotation<OpenVolumeMesh::Vec3d>(const VMesh&);
    template OpenVolumeMesh::Vec3f get_rotation<OpenVolumeMesh::Vec3f>(const VMesh&);
    template std::array<double, 3> get_rotation<std::array<double, 3>>(const VMesh&);
    template std::array<float, 3> get_rotation<std::array<float, 3>>(const VMesh&);


    template void set_camera_position<glm::vec3>(const glm::vec3&);
    template void set_camera_position<OpenVolumeMesh::Vec3d>(const OpenVolumeMesh::Vec3d&);
    template void set_camera_position<OpenVolumeMesh::Vec3f>(const OpenVolumeMesh::Vec3f&);
    template void set_camera_position<std::array<double, 3>>(const std::array<double, 3>&);
    template void set_camera_position<std::array<float, 3>>(const std::array<float, 3>&);

    template void set_camera_view_direction<glm::vec3>(const glm::vec3&);
    template void set_camera_view_direction<OpenVolumeMesh::Vec3d>(const OpenVolumeMesh::Vec3d&);
    template void set_camera_view_direction<OpenVolumeMesh::Vec3f>(const OpenVolumeMesh::Vec3f&);
    template void set_camera_view_direction<std::array<double, 3>>(const std::array<double, 3>&);
    template void set_camera_view_direction<std::array<float, 3>>(const std::array<float, 3>&);


    template VBox add_shape<VBox>();
    template VCylinder add_shape<VCylinder>();
    template VSphere add_shape<VSphere>();
    template VCone add_shape<VCone>();
    template VArrow add_shape<VArrow>();

    template VBox add_shape<VBox>(const VMesh&);
    template VCylinder add_shape<VCylinder>(const VMesh&);
    template VSphere add_shape<VSphere>(const VMesh&);
    template VCone add_shape<VCone>(const VMesh&);
    template VArrow add_shape<VArrow>(const VMesh&);

    template VBox add_shape<VBox>(const VMesh&, OpenVolumeMesh::CellHandle);
    template VCylinder add_shape<VCylinder>(const VMesh&, OpenVolumeMesh::CellHandle);
    template VSphere add_shape<VSphere>(const VMesh&, OpenVolumeMesh::CellHandle);
    template VCone add_shape<VCone>(const VMesh&, OpenVolumeMesh::CellHandle);
    template VArrow add_shape<VArrow>(const VMesh&, OpenVolumeMesh::CellHandle);

    template void set_position<glm::vec3>(const VShape&, const glm::vec3&);
    template void set_position<OpenVolumeMesh::Vec3d>(const VShape&, const OpenVolumeMesh::Vec3d&);
    template void set_position<OpenVolumeMesh::Vec3f>(const VShape&, const OpenVolumeMesh::Vec3f&);
    template void set_position<std::array<double, 3>>(const VShape&, const std::array<double, 3>&);
    template void set_position<std::array<float, 3>>(const VShape&, const std::array<float, 3>&);

    template void set_direction<glm::vec3>(const VShape&, const glm::vec3&, float);
    template void set_direction<OpenVolumeMesh::Vec3d>(const VShape&, const OpenVolumeMesh::Vec3d&, float);
    template void set_direction<OpenVolumeMesh::Vec3f>(const VShape&, const OpenVolumeMesh::Vec3f&, float);
    template void set_direction<std::array<double, 3>>(const VShape&, const std::array<double, 3>&, float);
    template void set_direction<std::array<float, 3>>(const VShape&, const std::array<float, 3>&, float);

    template void set_scale<glm::vec3>(const VShape&, const glm::vec3&);
    template void set_scale<OpenVolumeMesh::Vec3d>(const VShape&, const OpenVolumeMesh::Vec3d&);
    template void set_scale<OpenVolumeMesh::Vec3f>(const VShape&, const OpenVolumeMesh::Vec3f&);
    template void set_scale<std::array<double, 3>>(const VShape&, const std::array<double, 3>&);
    template void set_scale<std::array<float, 3>>(const VShape&, const std::array<float, 3>&);

    template void set_color<glm::vec4>(const VShape&, const glm::vec4&);
    template void set_color<OpenVolumeMesh::Vec4d>(const VShape&, const OpenVolumeMesh::Vec4d&);
    template void set_color<OpenVolumeMesh::Vec4f>(const VShape&, const OpenVolumeMesh::Vec4f&);
    template void set_color<std::array<double, 4>>(const VShape&, const std::array<double, 4>&);
    template void set_color<std::array<float, 4>>(const VShape&, const std::array<float, 4>&);

    template glm::vec3 get_position<glm::vec3>(const VShape&);
    template OpenVolumeMesh::Vec3d get_position<OpenVolumeMesh::Vec3d>(const VShape&);
    template OpenVolumeMesh::Vec3f get_position<OpenVolumeMesh::Vec3f>(const VShape&);
    template std::array<double, 3> get_position<std::array<double, 3>>(const VShape&);
    template std::array<float, 3> get_position<std::array<float, 3>>(const VShape&);

    template glm::vec3 get_scale<glm::vec3>(const VShape&);
    template OpenVolumeMesh::Vec3d get_scale<OpenVolumeMesh::Vec3d>(const VShape&);
    template OpenVolumeMesh::Vec3f get_scale<OpenVolumeMesh::Vec3f>(const VShape&);
    template std::array<double, 3> get_scale<std::array<double, 3>>(const VShape&);
    template std::array<float, 3> get_scale<std::array<float, 3>>(const VShape&);
}