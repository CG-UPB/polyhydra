
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

namespace volumeshOS
{
    static std::vector<std::function<void()>> commands     = {};
    static std::unique_ptr<Internal::Window> window        = nullptr;
    static std::shared_ptr<Internal::MeshList> mesh_list   = nullptr;
    static std::shared_ptr<Internal::Camera> camera        = nullptr;

    void initialize()
    {
        window = std::make_unique<Internal::Window>(1280, 720, "volumeshOS");
        window->initialize();
        mesh_list = window->panels.mesh_view->renderer->mesh_list;
        camera = window->panels.mesh_view->renderer->camera;
    }

    void clean_up()
    {
        camera = nullptr;
        mesh_list = nullptr;
        window->clean_up();
        window = nullptr;
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


    VMesh load(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* instance)
    {
        int id = mesh_list->next_id();
        VMesh vmesh(id);
        commands.emplace_back([id, instance]{
            mesh_list->add_mesh(id, instance);
            focus(VMesh(id));
        });
        return vmesh;
    }

    VMesh load(const std::string& path)
    {
        int id = mesh_list->next_id();
        VMesh vmesh(id);
        commands.emplace_back([id, path]{
            mesh_list->add_mesh(0, path);
        });
        return vmesh;
    }

    VMesh load(const char* path)
    {
        int id = mesh_list->next_id();
        VMesh vmesh(id);
        commands.emplace_back([id, path]{
            mesh_list->add_mesh(id, path);
        });
        return vmesh;
    }

    VMesh load_from_dialog(const std::string& title)
    {
        if (auto file = volumeshOS::file_dialog(title))
        {
            return volumeshOS::load(file);
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
        return mesh.get_id() >= 0;
    }

    void load_configuration(const VMesh& mesh, const std::string& path)
    {

    }

    void save_configuration(const VMesh& mesh, const std::string& path)
    {

    }

    void set_color(const Color& color)
    {
        commands.emplace_back([color]{
            mesh_list->set_color(color);
        });
    }

    void set_color(const VMesh& mesh, const Color& color)
    {
        commands.emplace_back([mesh, color]{
            mesh_list->set_color(mesh.get_id(), color);
        });
    }

    void set_color(const VMesh& mesh, OpenVolumeMesh::CellHandle cell, const Color& color)
    {
        commands.emplace_back([mesh, cell, color]{
            mesh_list->set_color(mesh.get_id(), cell, color);
        });
    }

    void set_color(const VMesh& mesh, OpenVolumeMesh::FaceHandle face, const Color& color)
    {
        commands.emplace_back([mesh, face, color]{
            mesh_list->set_color(mesh.get_id(), face, color);
        });
    }

    /*
    void set_color(const VMesh& mesh, OpenVolumeMesh::HalfFaceHandle halfface, const Color& color)
    {
        commands.emplace_back([mesh, halfface, color]{
            mesh_list->set_color(mesh.get_id(), halfface, color);
        });
    }

    void set_color(const VMesh& mesh, OpenVolumeMesh::EdgeHandle edge, const Color& color)
    {
        commands.emplace_back([mesh, edge, color]{
            mesh_list->set_color(mesh.get_id(), edge, color);
        });
    }

    void set_color(const VMesh& mesh, OpenVolumeMesh::VertexHandle vertex, const Color& color)
    {
        commands.emplace_back([mesh, vertex, color]{
            mesh_list->set_color(mesh.get_id(), vertex, color);
        });
    }
    */

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

    bool activate_rounding(const VMesh& mesh, bool rounding)
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

    void set_focused_mesh(const VMesh& mesh)
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

    void set_camera_view_direction(float x, float y, float z)
    {
        commands.emplace_back([x, y, z]{
            camera->set_view_direction(glm::vec3(x, y, z));
        });
    }

    void set_camera_mode(Mode mode)
    {
        commands.emplace_back([mode]{
            camera->set_mode(mode);
        });
    }

    void focus(const VMesh& mesh)
    {
        if(auto mesh_obj = mesh_list->get_mesh(mesh.get_id()))
        {
            auto pos = mesh_obj->get_data().position;
            commands.emplace_back([pos](){
                camera->look_at(pos);
            });
        }
    }

    void export_image()
    {

    }

    void export_image(const std::string& path)
    {

    }

    void log(const std::string& message)
    {
        Internal::Log::info(message);
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

    std::array<float, 3> get_position(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_position(mesh.get_id());
    }

    float get_scale(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_scale(mesh.get_id());
    }

    std::array<float, 3> get_rotation(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_rotation(mesh.get_id());
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

    Color get_color(const VMesh& mesh)
    {
        return mesh_list->get_color(mesh.get_id());
    }

    Color get_color(const VMesh& mesh, OpenVolumeMesh::CellHandle cell)
    {
        return mesh_list->get_color(mesh.get_id(), cell);
    }

    Color get_color(const VMesh& mesh, OpenVolumeMesh::HalfFaceHandle halfface)
    {
        return mesh_list->get_color(mesh.get_id(), halfface);
    }

    /*
    Color get_color(const VMesh& mesh, OpenVolumeMesh::EdgeHandle edge)
    {
        return mesh_list->get_color(mesh.get_id(), edge);
    }

    Color get_color(const VMesh& mesh, OpenVolumeMesh::VertexHandle vertex)
    {
        return mesh_list->get_color(mesh.get_id(), vertex);
    }
    */

}