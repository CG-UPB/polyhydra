#define VOS_IGNORE_INVALID_MESH
#ifndef VOS_IGNORE_INVALID_MESH
    #define VOS_ASSERT_VALID_MESH(mesh, list) assert(list->get_mesh(mesh.get_id()) && "Invalid VMesh handle")
#else
    #define VOS_ASSERT_VALID_MESH(mesh, list) if (!list->get_mesh(mesh.get_id())) return
#endif

#include "vospch.h"

#include <OpenVolumeMesh/Mesh/TetrahedralMeshTopologyKernel.hh>
#include <OpenVolumeMesh/Mesh/HexahedralMeshTopologyKernel.hh>

#include <memory>
#include <ctime>

#include "volumeshOS.h"

#include "Window.h"
#include "mesh/MeshList.h"
#include "panels/NewFileDialog.h"
#include "settings/AppState.h"
#include "mesh/MeshSerializer.h"

namespace volumeshOS
{
    static std::vector<std::function<void()>> commands     = {};
    static std::unique_ptr<Internal::Window> window        = nullptr;
    static std::shared_ptr<Internal::MeshList> mesh_list   = nullptr;
    static std::shared_ptr<Internal::Camera> camera        = nullptr;
    static std::shared_ptr<Internal::ShapeRenderer> shapes = nullptr;
    static int current_mesh_id                             = 0;
    static int current_shape_id                            = 0;

    void init()
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
        init();

        // render loop
        while (!window->should_close())
        {
            window->render();
            execute_commands();
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

    template<typename KernelType>
    VMesh load(const OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d, KernelType>* instance, const char* name)
    {
        static_assert(std::is_base_of_v<OpenVolumeMesh::TopologyKernel, KernelType>);
        int id = next_mesh_id();
        VMesh vmesh(id);
        auto mesh = std::make_shared<OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>>();
        mesh->assign(instance);

        commands.emplace_back([id, mesh, name]{
            mesh_list->add_mesh(id, mesh);
            if (name != nullptr)
            {
                mesh_list->set_name(id, name);
            }
        });
        focus_camera_on_mesh(VMesh(id));
        vmesh.set_lighting_mode(static_cast<LightingMode>(Internal::AppState::settings.use_global_pbr));
        return vmesh;
    }

    VMesh load(const std::string& path, const char* name)
    {
        int id = next_mesh_id();
        VMesh vmesh(id);
        Internal::AppState::settings.reading_file = true;
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
        focus_camera_on_mesh(VMesh(id));

        vmesh.set_lighting_mode(static_cast<LightingMode>(Internal::AppState::settings.use_global_pbr));
        return vmesh;
    }

    VMesh load(const char* path, const char* name)
    {
        int id = next_mesh_id();
        VMesh vmesh(id);
        Internal::AppState::settings.reading_file = true;
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
        focus_camera_on_mesh(VMesh(id));
        vmesh.set_lighting_mode(static_cast<LightingMode>(Internal::AppState::settings.use_global_pbr));

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

    void set_focused_mesh(const VMesh& mesh)
    {
        commands.emplace_back([mesh](){
            mesh_list->set_focused_mesh(mesh.get_id());
        });
    }

    VMesh get_focused_mesh()
    {
        if(auto m = mesh_list->get_focused_mesh())
        {
            return VMesh(m->get_id());
        }
        else
        {
            return VMesh(-1);
        }
    }

    void set_name(const VMesh& mesh, const std::string& name)
    {
        commands.emplace_back([mesh, name]{
            mesh_list->set_name(mesh.get_id(), name);
        });
    }

    [[nodiscard]] const std::string& get_name(const VMesh& mesh)
    {
        return mesh_list->get_name(mesh.get_id());
    }

    void set_rendering_mode(const VMesh& mesh, RenderingMode mode)
    {
        commands.emplace_back([mesh, mode]{
            mesh_list->set_rendering_mode(mesh.get_id(), mode);
        });
    }

    void render_cells(const VMesh& mesh, bool cells)
    {
        commands.emplace_back([mesh, cells]{
            mesh_list->render_cells(mesh.get_id(), cells);
        });
    }

    void render_lines(const VMesh& mesh, bool lines)
    {
        commands.emplace_back([mesh, lines]{
            mesh_list->render_lines(mesh.get_id(), lines);
        });
    }

    void render_points(const VMesh& mesh, bool points)
    {
        commands.emplace_back([mesh, points]{
            mesh_list->render_points(mesh.get_id(), points);
        });
    }

    template<typename Vec3T>
    void set_line_color(const VMesh& mesh, const Vec3T& color)
    {
        commands.emplace_back([mesh, color]{
            auto col = Internal::to_glm_vec3(color);
            mesh_list->set_line_color(mesh.get_id(), col);
        });
    }


    template<typename Vec3T>
    void set_point_color(const VMesh& mesh, const Vec3T& color)
    {
        commands.emplace_back([mesh, color]{
            auto col = Internal::to_glm_vec3(color);
            mesh_list->set_point_color(mesh.get_id(), col);
        });
    }

    bool is_rendering_cells(const VMesh& mesh)
    {
        return mesh_list->is_rendering_cells(mesh.get_id());
    }

    bool is_rendering_lines(const VMesh& mesh)
    {
        return mesh_list->is_rendering_lines(mesh.get_id());
    }

    bool is_rendering_points(const VMesh& mesh)
    {
        return mesh_list->is_rendering_points(mesh.get_id());
    }

    template<typename Vec3T>
    Vec3T get_line_color(const VMesh& mesh)
    {
        return Internal::glm_vec3_to<Vec3T>(mesh_list->get_line_color(mesh.get_id()));
    }

    template<typename Vec3T>
    Vec3T get_point_color(const VMesh& mesh)
    {
        return Internal::glm_vec3_to<Vec3T>(mesh_list->get_point_color(mesh.get_id()));

    }

    void set_point_size(const VMesh& mesh, float size)
    {
        commands.emplace_back([mesh, size]{
            mesh_list->set_point_size(mesh.get_id(), size);
        });
    }

    float get_point_size(const VMesh& mesh)
    {
        return mesh_list->get_point_size(mesh.get_id());
    }

    void set_line_width(const VMesh& mesh, float width)
    {
        commands.emplace_back([mesh, width]{
            mesh_list->set_line_width(mesh.get_id(), width);
        });
    }

    float get_line_width(const VMesh& mesh)
    {
        return mesh_list->get_line_width(mesh.get_id());
    }

    void set_shading_mode(const VMesh& mesh, ShadingMode mode)
    {
        commands.emplace_back([mesh, mode]{
            mesh_list->set_shading_mode(mesh.get_id(), mode);
        });
    }

    ShadingMode get_shading_mode(const VMesh& mesh)
    {
        return mesh_list->get_shading_mode(mesh.get_id());
    }

    template<typename Vec3T>
    void set_sky_color(const Vec3T& color)
    {
        commands.emplace_back([color]{
            auto col = Internal::to_glm_vec3(color);
            Internal::AppState::settings.sky.sky_color = col;
        });
    }

    template<typename Vec3T>
    Vec3T get_sky_color()
    {
        return Internal::glm_vec3_to<Vec3T>(Internal::AppState::settings.sky.sky_color);
    }

    void set_fog_density(float density)
    {
        commands.emplace_back([density]{
            std::clamp(density, 0.0f, 1.0f);
            Internal::AppState::settings.sky.fog_density = density;
        });
    }

    float get_fog_density()
    {
        return Internal::AppState::settings.sky.fog_density;
    }

    template<typename Vec3T>
    void set_fog_color(const Vec3T& color)
    {
        commands.emplace_back([color]{
            auto col = Internal::to_glm_vec3(color);
            Internal::AppState::settings.sky.fog_color = col;
        });
    }

    template<typename Vec3T>
    Vec3T get_fog_color()
    {
        return Internal::glm_vec3_to<Vec3T>(Internal::AppState::settings.sky.fog_color);
    }

    void set_selection_mode(SelectionMode mode)
    {
        commands.emplace_back([mode]{
            Internal::AppState::settings.selection_mode = mode;
        });
    }

    SelectionMode get_selection_mode()
    {
        return Internal::AppState::settings.selection_mode;
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

    void set_camera_mode(CameraMode mode)
    {
        commands.emplace_back([mode]{
            camera->set_mode(mode);
            Internal::AppState::settings.camera.mode = mode;
        });
    }

    CameraMode get_camera_mode()
    {
        return camera->get_mode();
    }

    void set_camera_position(float x, float y, float z)
    {
        commands.emplace_back([x, y, z]{
            camera->set_position(glm::vec3(x, y, z));
            Internal::AppState::settings.camera.position = glm::vec3(x, y, z);
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

    template<typename Vec3T>
    Vec3T get_camera_position()
    {
        return Internal::glm_vec3_to<Vec3T>(camera->get_position());
    }

    void set_camera_target(float x, float y, float z)
    {
        commands.emplace_back([x, y, z]{
            camera->look_at(glm::vec3(x, y, z));
        });
    }

    template<typename Vec3T>
    void set_camera_target(const Vec3T& position)
    {
        commands.emplace_back([position]{
            auto pos = Internal::to_glm_vec3(position);
            camera->set_target(pos);
        });
    }

    template<typename Vec3T>
    Vec3T get_camera_target()
    {
        return Internal::glm_vec3_to<Vec3T>(camera->get_target());
    }

    void focus_camera_on_mesh(const VMesh& mesh)
    {
        commands.emplace_back([mesh]{
            if(auto mesh_obj = mesh_list->get_mesh(mesh.get_id()))
            {
                auto pos = mesh_obj->get_data().position;
                camera->animated_look_at(pos);
            }
        });
    }

    void set_camera_fov(float fov)
    {
        assert(fov >= 1.0 && fov <= 90.0);
        commands.emplace_back([fov]{
            camera->zoom = fov;
            Internal::AppState::settings.camera.fov = fov;
        });
    }

    float get_camera_fov()
    {
        return camera->zoom;
    }

    void set_light_direction(float x, float y, float z)
    {
        commands.emplace_back([x, y, z]{
            auto dir = glm::normalize(glm::vec3(x,y,z));
            Internal::AppState::settings.light.direction = dir;
        });
    }

    // Set the direction of the light in the scene
    template<typename Vec3T>
    void set_light_direction(const Vec3T& direction)
    {
        commands.emplace_back([direction]{
            auto dir = Internal::to_glm_vec3(direction);
            dir.r = std::clamp(dir.r, -1.0f, 1.0f);
            dir.g = std::clamp(dir.g, -1.0f, 1.0f);
            dir.b = std::clamp(dir.b, -1.0f, 1.0f);

            Internal::AppState::settings.light.direction = dir;
        });
    }

    template<typename Vec3T>
    Vec3T& get_light_direction()
    {
        return Internal::AppState::settings.light.direction;
    }

    // Set light color
    template<typename Vec3T>
    void set_light_color(const Vec3T& color)
    {
        commands.emplace_back([color]{
            auto col = Internal::to_glm_vec3(color);
            Internal::AppState::settings.light.color = col;
        });
    }

    template<typename Vec3T>
    Vec3T& get_light_color()
    {
        return Internal::AppState::settings.light.color;
    }

    void set_light_intensity(float intensity)
    {
        commands.emplace_back([intensity]{
            Internal::AppState::settings.light.intensity = intensity;
        });
    }

    float get_light_intensity()
    {
        return Internal::AppState::settings.light.intensity;
    }

    void set_gamma(float gamma)
    {
        commands.emplace_back([gamma]{
            Internal::AppState::settings.post_processing.gamma = gamma;
        });
    }

    float get_gamma()
    {
        return Internal::AppState::settings.post_processing.gamma;
    }

    void set_saturation(float saturation)
    {
        commands.emplace_back([saturation]{
            Internal::AppState::settings.post_processing.saturation = saturation;
        });
    }

    float get_saturation()
    {
        return Internal::AppState::settings.post_processing.saturation;
    }

    void set_contrast(float contrast)
    {
        commands.emplace_back([contrast]{
            Internal::AppState::settings.post_processing.contrast = contrast;
        });
    }

    float get_contrast()
    {
        return Internal::AppState::settings.post_processing.contrast;
    }

    void use_ground(bool ground)
    {
        commands.emplace_back([ground]{
            Internal::AppState::settings.ground.solid = ground;
        });
    }

    bool is_using_ground()
    {
        return Internal::AppState::settings.ground.solid;
    }

    void use_grid(bool grid)
    {
        commands.emplace_back([grid]{
            Internal::AppState::settings.ground.grid = grid;
        });
    }

    bool is_using_grid()
    {
        return Internal::AppState::settings.ground.grid;
    }

    template<typename Vec3T>
    void set_ground_color(const Vec3T& color)
    {
        commands.emplace_back([color]{
            auto col = Internal::to_glm_vec3(color);
            Internal::AppState::settings.ground.solid_color = col;
        });
    }

    template<typename Vec3T>
    Vec3T& get_ground_color()
    {
        return Internal::AppState::settings.ground.solid_color;
    }

    template<typename Vec3T>
    void set_grid_color(const Vec3T& color)
    {
        commands.emplace_back([color]{
            auto col = Internal::to_glm_vec3(color);
            Internal::AppState::settings.ground.grid_color = col;
        });
    }

    template<typename Vec3T>
    Vec3T& get_grid_color()
    {
        return Internal::AppState::settings.ground.grid_color;
    }

    void set_ground_height(float height)
    {
        commands.emplace_back([height]{
            Internal::AppState::settings.ground.height = height;
        });
    }

    float get_gound_height()
    {
        return Internal::AppState::settings.ground.height;
    }

    void use_shadows(bool shadows)
    {
        commands.emplace_back([shadows]{
            Internal::AppState::settings.shadows_active = shadows;
        });
    }

    bool is_using_shadows()
    {
        return Internal::AppState::settings.shadows_active;
    }

    void set_shadow_cascades(int cascades)
    {
        cascades = std::clamp(cascades, 0, 8);
        commands.emplace_back([cascades]{
            Internal::AppState::settings.num_shadow_cascades = cascades;
        });
    }

    int get_shadow_cascades()
    {
        return Internal::AppState::settings.num_shadow_cascades;
    }

    void set_shadow_strength(float strength)
    {
        commands.emplace_back([strength]{
            Internal::AppState::settings.shadow.shadow_strength = strength;
        });
    }

    float get_shadow_strength()
    {
        return Internal::AppState::settings.shadow.shadow_strength;
    }

    void set_shadow_penumbra(float penumbra)
    {
        commands.emplace_back([penumbra]{
            Internal::AppState::settings.shadow.penumbra_scale = penumbra;
        });
    }

    float get_shadow_penumbra()
    {
        return Internal::AppState::settings.shadow.penumbra_scale;
    }


    void use_ambient_occlusion( bool ssao)
    {
        commands.emplace_back([ssao]{
            Internal::AppState::settings.ssao_active = ssao;
        });
    }

    bool is_using_ambient_occlusion()
    {
        return Internal::AppState::settings.ssao_active;
    }

    void set_ambient_occlusion_preset(SSAOMode mode)
    {
        commands.emplace_back([mode]{
            Internal::AppState::settings.ssao_mode = mode;
        });
    }

    bool get_ambient_occlusion_preset()
    {
        return Internal::AppState::settings.ssao_active;
    }


    void use_transparency( bool transparency)
    {
        commands.emplace_back([transparency]{
            Internal::AppState::settings.transparency_active = transparency;
        });
    }

    bool is_using_transparency()
    {
        return Internal::AppState::settings.transparency_active;
    }

    void set_transparency_mode(TransparencyMode mode)
    {
        commands.emplace_back([mode]{
            Internal::AppState::settings.transparency_mode = mode;
        });
    }

    TransparencyMode set_transparency_mode()
    {
        return Internal::AppState::settings.transparency_mode;
    }

    void set_transparency_passes(int passes)
    {
        passes = std::clamp(passes, 0, 100);
        commands.emplace_back([passes]{
            Internal::AppState::settings.num_depth_peeling_passes = passes;
        });
    }

    int set_transparency_passes()
    {
        return Internal::AppState::settings.num_depth_peeling_passes;
    }

    void update(const VMesh& mesh, OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* instance)
    {
        auto ovm = std::make_shared<OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>>();
        ovm->assign(instance);
        commands.emplace_back([mesh, ovm]{
            mesh_list->set_mesh(mesh.get_id(), ovm);
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

    bool is_valid(const VMesh& mesh)
    {
        return mesh.get_id() >= 0 && mesh_list->get_mesh(mesh.get_id()) != nullptr;
    }

    bool is_bezier_mesh(const VMesh& mesh)
    {
        std::shared_ptr<Internal::MeshObject> mesh_obj = mesh_list->get_mesh(mesh.get_id());
        return mesh_obj != nullptr && mesh_obj->is_bezier_mesh();
    }

    [[nodiscard]] OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* get_ovm(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_mesh(mesh.get_id())->get_ovm().get();
    }

    void load_configuration(const VMesh& mesh, const std::string& path)
    {
        commands.emplace_back([mesh, path]{
            assert(mesh.is_valid());
            auto mesh_object = mesh_list->get_mesh(mesh.get_id());
            Internal::MeshSerializer::read_from_file(*mesh_object, path);
        });
    }

    void save_configuration(const VMesh& mesh, const std::string& path)
    {
        commands.emplace_back([mesh, path]{
            assert(mesh.is_valid());
            auto mesh_object = mesh_list->get_mesh(mesh.get_id());
            Internal::MeshSerializer::write_to_file(*mesh_object, path);
        });
    }

    void use_backface_culling(const VMesh& mesh, bool culling)
    {

        commands.emplace_back([mesh, culling]{
            mesh_list->get_mesh(mesh.get_id())->get_data().use_back_face_culling = culling;
        });
    }

    bool is_using_backface_culling(const VMesh& mesh)
    {
        return mesh_list->get_mesh(mesh.get_id())->get_data().use_back_face_culling;
    }

    void use_two_sided_lighting(const VMesh& mesh, bool ts_lighting)
    {
        commands.emplace_back([mesh, ts_lighting]{
            mesh_list->get_mesh(mesh.get_id())->get_data().use_two_sided_lighting = ts_lighting;
        });
    }

    bool is_using_two_sided_lighting(const VMesh& mesh)
    {
        return mesh_list->get_mesh(mesh.get_id())->get_data().use_two_sided_lighting;
    }

    void use_base_color(const VMesh& mesh, bool base_color)
    {
        commands.emplace_back([mesh, base_color]{
            mesh_list->get_mesh(mesh.get_id())->get_data().use_base_color = base_color;
        });
    }

    bool is_using_base_color(const VMesh& mesh)
    {
        return mesh_list->get_mesh(mesh.get_id())->get_data().use_base_color;
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

    void reset_selection(const VMesh& mesh)
    {
        commands.emplace_back([mesh]{
            mesh_list->reset_selection(mesh.get_id());
        });
    }

    void set_lighting_mode(const VMesh& mesh, LightingMode mode)
    {
        commands.emplace_back([mesh, mode]{
            mesh_list->get_mesh(mesh.get_id())->get_data().use_pbr = mode == LightingMode::PBR;
        });
    }

    LightingMode get_lighting_mode(const VMesh& mesh)
    {
        return mesh_list->get_mesh(mesh.get_id())->get_data().use_pbr ? LightingMode::PBR : LightingMode::PHONG;
    }


    void set_ambient(const VMesh& mesh, float ambient)
    {
        commands.emplace_back([mesh, ambient]{
            mesh_list->set_ambient(mesh.get_id(), ambient);
        });
    }

    float get_ambient(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_ambient(mesh.get_id());
    }

    void set_diffuse(const VMesh& mesh, float diffuse)
    {
        commands.emplace_back([mesh, diffuse]{
            mesh_list->set_diffuse(mesh.get_id(), diffuse);
        });
    }

    float get_diffuse(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_diffuse(mesh.get_id());
    }

    void set_specular(const VMesh& mesh, float specular)
    {
        commands.emplace_back([mesh, specular]{
            mesh_list->set_specular(mesh.get_id(), specular);
        });
    }

    float get_specular(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_specular(mesh.get_id());
    }

    void set_specular_coefficient(const VMesh& mesh, float coefficient)
    {
        commands.emplace_back([mesh, coefficient]{
            mesh_list->set_specular_coefficient(mesh.get_id(), coefficient);
        });
    }

    float get_specular_coefficient(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_specular_coefficient(mesh.get_id());
    }

    void set_metallic(const VMesh& mesh, float metallic)
    {
        commands.emplace_back([mesh, metallic]{
            mesh_list->get_mesh(mesh.get_id())->get_data().metallic = metallic;
        });
    }

    float get_metallic(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_mesh(mesh.get_id())->get_data().metallic;
    }

    void set_roughness(const VMesh& mesh, float roughness)
    {
        commands.emplace_back([mesh, roughness]{
            mesh_list->get_mesh(mesh.get_id())->get_data().roughness = roughness;
        });
    }

    float get_roughness(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_mesh(mesh.get_id())->get_data().roughness;
    }

    template<typename Vec3T>
    void set_origin(const VMesh& mesh, const Vec3T& origin)
    {
        commands.emplace_back([mesh, origin]{
            auto pos = Internal::to_glm_vec3(origin);
            mesh_list->set_origin(mesh.get_id(), pos);
        });
    }

    template<typename Vec3T>
    Vec3T get_origin(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return Internal::glm_vec3_to<Vec3T>(mesh_list->get_origin(mesh.get_id()));
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

    template<typename Vec3T>
    Vec3T get_position(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return Internal::glm_vec3_to<Vec3T>(mesh_list->get_position(mesh.get_id()));
    }

    void set_scale(const VMesh& mesh, float scale)
    {
        commands.emplace_back([mesh, scale]{
            mesh_list->set_scale(mesh.get_id(), scale);
        });
    }

    float get_scale(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_scale(mesh.get_id());
    }

    void use_scale_normalization(const VMesh& mesh, bool use_scale_norm)
    {
        commands.emplace_back([mesh, use_scale_norm]{
            mesh_list->use_scale_normalization(mesh.get_id(), use_scale_norm);
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

    template<typename Vec3T>
    void set_rotation(const VMesh& mesh, float angle, const Vec3T& axis)
    {
        commands.emplace_back([mesh, angle, axis]{
            auto ax = Internal::to_glm_vec3(axis);
            mesh_list->set_rotation(mesh.get_id(), angle, ax);
        });
    }

    template<typename Vec3T>
    Vec3T get_rotation(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return Internal::glm_vec3_to<Vec3T>(mesh_list->get_rotation(mesh.get_id()));
    }

    template <typename Vec3T>
    Vec3T get_transformed_point(const VMesh& mesh, const Vec3T& point)
    {
        assert(mesh.is_valid());
        return Internal::glm_vec3_to<Vec3T>(glm::vec3(mesh_list->get_mesh(mesh.get_id())->get_data().get_transform()
                                                    * glm::vec4(Internal::to_glm_vec3(point), 1)));
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

    float get_slice_factor(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_slice_factor(mesh.get_id());
    }

    void set_slice_locked(const VMesh& mesh, bool locked)
    {
        commands.emplace_back([mesh, locked]{
            mesh_list->set_slice_lock(mesh.get_id(), locked);
        });
    }

    bool get_slice_locked(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_slice_lock(mesh.get_id());
    }

    void set_peel_level(const VMesh& mesh, const float level)
    {
        commands.emplace_back([mesh, level]{
            mesh_list->set_peel_level(mesh.get_id(), level);
        });
    }

    float get_peel_level(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_peel_level(mesh.get_id());
    }

    void use_reverse_peeling(const VMesh& mesh, bool reverse)
    {
        commands.emplace_back([mesh, reverse]{
            mesh_list->set_reverse_peeling(mesh.get_id(), reverse);
        });
    }

    bool is_using_reverse_peeling(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_reverse_peeling(mesh.get_id());
    }

    int get_max_peel_depth(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_max_peel_depth(mesh.get_id());
    }

    void set_cell_rounding(const VMesh& mesh, float rounding)
    {
        commands.emplace_back([mesh, rounding]{
            mesh_list->set_cell_rounding(mesh.get_id(), rounding);
        });
    }

    float get_cell_rounding(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_cell_rounding(mesh.get_id());
    }

    void set_tessellation_level(const VMesh& mesh, int level)
    {
        commands.emplace_back([mesh, level]{
            mesh_list->set_tessellation_level(mesh.get_id(), level);
        });
    }

    int get_tessellation_level(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_tessellation_level(mesh.get_id());
    }


    void set_cell_size(const VMesh& mesh, const float size)
    {
        commands.emplace_back([mesh, size]{
            mesh_list->set_cell_size(mesh.get_id(), size);
        });
    }

    float get_cell_size(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_cell_size(mesh.get_id());
    }

    void set_visibility(const VMesh& mesh, OpenVolumeMesh::CellHandle cell, bool visible)
    {
        commands.emplace_back([mesh, cell, visible](){
            mesh_list->set_visibility(mesh.get_id(), cell, visible);
        });
    }

    bool get_visibility(const VMesh& mesh)
    {
        assert(mesh.is_valid());
        return mesh_list->get_visibility(mesh.get_id());
    }

    void set_visibility(const VMesh& mesh, const bool visible)
    {
        commands.emplace_back([mesh, visible]{
            mesh_list->set_visibility(mesh.get_id(), visible);
        });
    }

    bool get_visibility(const VMesh& mesh, OpenVolumeMesh::CellHandle cell)
    {
        assert(mesh.is_valid());
        return mesh_list->get_visibility(mesh.get_id(), cell);
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

    void dig(const VMesh& mesh, OpenVolumeMesh::CellHandle cell)
    {
        commands.emplace_back([mesh, cell]{
            mesh_list->hide(mesh.get_id(), cell);
        });
    }

    void never_discard(const VMesh& mesh, OpenVolumeMesh::CellHandle cell)
    {
        commands.emplace_back([mesh, cell]{
            mesh_list->never_discard(mesh.get_id(), cell);
        });
    }

    void block_inputs( bool block)
    {
        Internal::AppState::settings.block_input = block;
    }

    bool is_input_blocked()
    {
        return Internal::AppState::settings.block_input;
    }

    const char* file_dialog(const std::string& title)
    {
        Internal::NewFileDialog dialog;
        return dialog.open_dialog(title.c_str());
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

    int get_viewport_width()
    {
        return window->panels.mesh_view->renderer->frame.width;
    }

    int get_viewport_height()
    {
        return window->panels.mesh_view->renderer->frame.height;
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

    void use_log_window(bool use)
    {
        commands.emplace_back([use]{
            window->panels.mesh_view->log_window->hide_log_window(!use);
        });
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
        static_assert(std::is_base_of_v<VShape, ShapeType>);
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
        static_assert(std::is_base_of_v<VShape, ShapeType>);
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

    void set_shape_lighting_mode(LightingMode mode)
    {
        commands.emplace_back([mode]{
            Internal::AppState::settings.shapes.use_pbr = static_cast<bool>(mode);
        });
    }

    LightingMode get_shape_lighting_mode()
    {
        return Internal::AppState::settings.shapes.use_pbr ? LightingMode::PBR : LightingMode::PHONG;
    }

    void set_shape_ambient(float ambient)
    {
        commands.emplace_back([ambient]{
            Internal::AppState::settings.shapes.ambient_strength = ambient;
        });
    }

    float get_shape_ambient()
    {
        return Internal::AppState::settings.shapes.ambient_strength;
    }

    void set_shape_diffuse(float diffuse)
    {
        commands.emplace_back([diffuse]{
            Internal::AppState::settings.shapes.diffuse_strength = diffuse;
        });
    }

    float get_shape_diffuse()
    {
        return Internal::AppState::settings.shapes.diffuse_strength;
    }

    void set_shape_specular(float specular)
    {
        commands.emplace_back([specular]{
            Internal::AppState::settings.shapes.specular_strength = specular;
        });
    }

    float get_shape_specular()
    {
        return Internal::AppState::settings.shapes.specular_strength;
    }

    void set_shape_specular_coefficient(float coefficient)
    {
        commands.emplace_back([coefficient]{
            Internal::AppState::settings.shapes.specular_exponent = coefficient;
        });
    }

    float get_shape_specular_coefficient()
    {
        return Internal::AppState::settings.shapes.specular_exponent;
    }

    void set_shape_metallic(float metallic)
    {
        commands.emplace_back([metallic]{
            Internal::AppState::settings.shapes.metallic = metallic;
        });
    }

    float get_shape_metallic()
    {
        return Internal::AppState::settings.shapes.metallic;
    }

    void set_shape_roughness(float roughness)
    {
        commands.emplace_back([roughness]{
            Internal::AppState::settings.shapes.roughness = roughness;
        });
    }

    float get_shape_roughness()
    {
        return Internal::AppState::settings.shapes.roughness;
    }


    void set_theme(Theme theme)
    {
        commands.emplace_back([theme]{
            switch (theme)
            {
                case Theme::Light:
                    window->load_light_mode();
                    break;
                case Theme::Dark:
                    window->load_dark_mode();
                    break;
            }
        });
    }

    template VMesh load<OpenVolumeMesh::TopologyKernel>(const OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d, OpenVolumeMesh::TopologyKernel>*, const char*);
    template VMesh load<OpenVolumeMesh::TetrahedralMeshTopologyKernel>(const OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d, OpenVolumeMesh::TetrahedralMeshTopologyKernel>*, const char*);
    template VMesh load<OpenVolumeMesh::HexahedralMeshTopologyKernel>(const OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d, OpenVolumeMesh::HexahedralMeshTopologyKernel>*, const char*);


    template void set_line_color<glm::vec3>(const VMesh&, const glm::vec3&);
    template void set_line_color<OpenVolumeMesh::Vec3d>(const VMesh&, const OpenVolumeMesh::Vec3d&);
    template void set_line_color<OpenVolumeMesh::Vec3f>(const VMesh&, const OpenVolumeMesh::Vec3f&);
    template void set_line_color<std::array<double, 3>>(const VMesh&, const std::array<double, 3>&);
    template void set_line_color<std::array<float, 3>>(const VMesh&, const std::array<float, 3>&);

    template void set_point_color<glm::vec3>(const VMesh&, const glm::vec3&);
    template void set_point_color<OpenVolumeMesh::Vec3d>(const VMesh&, const OpenVolumeMesh::Vec3d&);
    template void set_point_color<OpenVolumeMesh::Vec3f>(const VMesh&, const OpenVolumeMesh::Vec3f&);
    template void set_point_color<std::array<double, 3>>(const VMesh&, const std::array<double, 3>&);
    template void set_point_color<std::array<float, 3>>(const VMesh&, const std::array<float, 3>&);

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

    template glm::vec3 get_line_color<glm::vec3>(const VMesh&);
    template OpenVolumeMesh::Vec3d get_line_color<OpenVolumeMesh::Vec3d>(const VMesh&);
    template OpenVolumeMesh::Vec3f get_line_color<OpenVolumeMesh::Vec3f>(const VMesh&);
    template std::array<double, 3> get_line_color<std::array<double, 3>>(const VMesh&);
    template std::array<float, 3> get_line_color<std::array<float, 3>>(const VMesh&);

    template glm::vec3 get_point_color<glm::vec3>(const VMesh&);
    template OpenVolumeMesh::Vec3d get_point_color<OpenVolumeMesh::Vec3d>(const VMesh&);
    template OpenVolumeMesh::Vec3f get_point_color<OpenVolumeMesh::Vec3f>(const VMesh&);
    template std::array<double, 3> get_point_color<std::array<double, 3>>(const VMesh&);
    template std::array<float, 3> get_point_color<std::array<float, 3>>(const VMesh&);

    template void set_origin<glm::vec3>(const VMesh&, const glm::vec3&);
    template void set_origin<OpenVolumeMesh::Vec3d>(const VMesh&, const OpenVolumeMesh::Vec3d&);
    template void set_origin<OpenVolumeMesh::Vec3f>(const VMesh&, const OpenVolumeMesh::Vec3f&);
    template void set_origin<std::array<double, 3>>(const VMesh&, const std::array<double, 3>&);
    template void set_origin<std::array<float, 3>>(const VMesh&, const std::array<float, 3>&);

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

    template void set_rotation<glm::vec3>(const VMesh&, float , const glm::vec3&);
    template void set_rotation<OpenVolumeMesh::Vec3d>(const VMesh&, float , const OpenVolumeMesh::Vec3d&);
    template void set_rotation<OpenVolumeMesh::Vec3f>(const VMesh&, float , const OpenVolumeMesh::Vec3f&);
    template void set_rotation<std::array<double, 3>>(const VMesh&, float , const std::array<double, 3>&);
    template void set_rotation<std::array<float, 3>>(const VMesh&, float , const std::array<float, 3>&);

    template glm::vec3 get_origin<glm::vec3>(const VMesh&);
    template OpenVolumeMesh::Vec3d get_origin<OpenVolumeMesh::Vec3d>(const VMesh&);
    template OpenVolumeMesh::Vec3f get_origin<OpenVolumeMesh::Vec3f>(const VMesh&);
    template std::array<double, 3> get_origin<std::array<double, 3>>(const VMesh&);
    template std::array<float, 3> get_origin<std::array<float, 3>>(const VMesh&);

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

    template glm::vec3 get_transformed_point<glm::vec3>(const VMesh& mesh, const glm::vec3& point);
    template OpenVolumeMesh::Vec3d get_transformed_point<OpenVolumeMesh::Vec3d>(const VMesh& mesh, const OpenVolumeMesh::Vec3d& point);
    template OpenVolumeMesh::Vec3f get_transformed_point<OpenVolumeMesh::Vec3f>(const VMesh& mesh, const OpenVolumeMesh::Vec3f& point);
    template std::array<double, 3> get_transformed_point<std::array<double, 3>>(const VMesh& mesh, const std::array<double, 3>& point);
    template std::array<float, 3> get_transformed_point<std::array<float, 3>>(const VMesh& mesh, const std::array<float, 3>& point);

    template void set_camera_position<glm::vec3>(const glm::vec3&);
    template void set_camera_position<OpenVolumeMesh::Vec3d>(const OpenVolumeMesh::Vec3d&);
    template void set_camera_position<OpenVolumeMesh::Vec3f>(const OpenVolumeMesh::Vec3f&);
    template void set_camera_position<std::array<double, 3>>(const std::array<double, 3>&);
    template void set_camera_position<std::array<float, 3>>(const std::array<float, 3>&);

    template glm::vec3 get_camera_position<glm::vec3>();
    template OpenVolumeMesh::Vec3d get_camera_position<OpenVolumeMesh::Vec3d>();
    template OpenVolumeMesh::Vec3f get_camera_position<OpenVolumeMesh::Vec3f>();
    template std::array<double, 3> get_camera_position<std::array<double, 3>>();
    template std::array<float, 3> get_camera_position<std::array<float, 3>>();

    template void set_camera_target<glm::vec3>(const glm::vec3&);
    template void set_camera_target<OpenVolumeMesh::Vec3d>(const OpenVolumeMesh::Vec3d&);
    template void set_camera_target<OpenVolumeMesh::Vec3f>(const OpenVolumeMesh::Vec3f&);
    template void set_camera_target<std::array<double, 3>>(const std::array<double, 3>&);
    template void set_camera_target<std::array<float, 3>>(const std::array<float, 3>&);

    template void set_light_direction<glm::vec3>(const glm::vec3&);
    template void set_light_direction<OpenVolumeMesh::Vec3d>(const OpenVolumeMesh::Vec3d&);
    template void set_light_direction<OpenVolumeMesh::Vec3f>(const OpenVolumeMesh::Vec3f&);
    template void set_light_direction<std::array<double, 3>>(const std::array<double, 3>&);
    template void set_light_direction<std::array<float, 3>>(const std::array<float, 3>&);

    template glm::vec3 get_camera_target<glm::vec3>();
    template OpenVolumeMesh::Vec3d get_camera_target<OpenVolumeMesh::Vec3d>();
    template OpenVolumeMesh::Vec3f get_camera_target<OpenVolumeMesh::Vec3f>();
    template std::array<double, 3> get_camera_target<std::array<double, 3>>();
    template std::array<float, 3> get_camera_target<std::array<float, 3>>();


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
