
#include "MeshList.h"
#include "util/Types.h"

namespace volumeshOS::Internal
{
    MeshList::MeshList()
    {
        m_total_meshes = 0;
        m_focused_mesh = -1;
    }

    void MeshList::add_mesh(MeshID mesh_id, const std::shared_ptr<OVMesh>& mesh)
    {

        auto new_mesh = std::make_shared<MeshObject>(mesh_id);
        //int x = mesh->n_vertices();
        new_mesh->set_mesh(mesh);

        // add mesh and its id to our list
        m_mesh_list.emplace(mesh_id, std::move(new_mesh));

        // select mesh as currently focused (for settings and camera)
        set_focused_mesh(mesh_id);


        // calculate the range of the selection ids per mesh
        calculate_selection_offsets();

    }

    void MeshList::add_mesh(MeshID mesh_id, const std::string& path)
    {
        auto ovm_mesh = std::make_shared<OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>>();
        OpenVolumeMesh::IO::FileManager file_manager;
        file_manager.readFile(path, *ovm_mesh, false);
        add_mesh(mesh_id, ovm_mesh);
    }

    void MeshList::set_mesh(const MeshID id, const std::shared_ptr<OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>>& ovm_mesh)
    {
        auto f = [&ovm_mesh](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->set_mesh(ovm_mesh);
        };

        execute_for_mesh(f, id);
    }

    void MeshList::set_mesh(const MeshID id, const std::string& path)
    {
        auto f = [&path](const std::shared_ptr<MeshObject>& mesh) -> void{
            auto ovm_mesh = std::make_shared<OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>>();
            OpenVolumeMesh::IO::FileManager file_manager;
            file_manager.readFile(path, *ovm_mesh);
            mesh->set_mesh(ovm_mesh);
        };

        execute_for_mesh(f, id);
    }

    void MeshList::delete_mesh(const MeshID id)
    {
        for(auto it = m_mesh_list.begin(); it!= m_mesh_list.end();)
        {
            if (it->first == id)
            {
                m_mesh_list.erase(it);
                m_total_meshes--;
                return;
            }
            else
            {
                ++it;
            }
        }
    }

    void MeshList::delete_meshes()
    {
        for(auto it = m_mesh_list.begin(); it!= m_mesh_list.end();)
        {
            m_mesh_list.erase(it);
        }
        m_total_meshes = 0;
    }


    void MeshList::set_focused_mesh(const MeshID id)
    {
        auto f = [this, id](const std::shared_ptr<MeshObject>& mesh) -> void{
            m_focused_mesh = id;
        };
        execute_for_mesh(f, id);
    }

    std::shared_ptr<MeshObject> MeshList::get_focused_mesh()
    {
        return get_mesh(m_focused_mesh);
    }

    MeshID MeshList::get_focused_mesh_id()
    {
        auto mesh = get_mesh(m_focused_mesh);
        if (mesh != nullptr)
        {
            return mesh->get_id();
        }
        return -1;
    }

    void MeshList::set_ambient(MeshID id, float ambient)
    {
        auto f = [ambient](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().ambient_strength = ambient;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_diffuse(MeshID id, float diffuse)
    {
        auto f = [diffuse](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().diffuse_strength = diffuse;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_specular(MeshID id, float specular)
    {
        auto f = [specular](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().specular_strength = specular;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_specular_coefficient(MeshID id, float coefficient)
    {
        auto f = [coefficient](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().specular_exponent = coefficient;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_phong(MeshID id, float ambient, float diffuse, float specular, float coefficient)
    {
        auto f = [ambient, diffuse, specular, coefficient](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().ambient_strength = ambient;
            mesh->get_data().diffuse_strength = diffuse;
            mesh->get_data().specular_strength = specular;
            mesh->get_data().specular_exponent = coefficient;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_position(MeshID id, float x, float y, float z)
    {
        auto f = [x, y, z](const std::shared_ptr<MeshObject>& mesh) -> void{
            //get_mesh(id)->get_data().position = glm::vec3(x, y, z);
            mesh->translate(glm::vec3(x, y, z));
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_scale(MeshID id, float scale)
    {
        auto f = [scale](const std::shared_ptr<MeshObject>& mesh) -> void{
            if(scale < 0.0f)
            {
                mesh->scale(glm::vec3(0.0f));
            }
            else
            {
                mesh->scale(glm::vec3(scale));
            }

        };
        execute_for_mesh(f, id);
    }

    void MeshList::use_scale_normalization(const MeshID id, bool use_scale_norm)
    {
        ;
        auto f = [use_scale_norm](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().use_scale_normalization = use_scale_norm;
            mesh->get_data().update_transform();
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_rotation(MeshID id, float x, float y, float z)
    {
        auto f = [this, &x, &y, &z, &id](const std::shared_ptr<MeshObject>& mesh) -> void{

            auto m = get_mesh(id);
            m->rotate_axis_delta(x,y,z);
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_rotation(MeshID id, float angle, glm::vec3& axis)
    {
        auto f = [this, &angle, &axis, &id](const std::shared_ptr<MeshObject>& mesh) -> void{

            auto m = get_mesh(id);
            m->rotate(angle, axis);
        };
        execute_for_mesh(f, id);
    }

    void MeshList::reset_rotation(MeshID id)
    {
        auto f = [this, &id](const std::shared_ptr<MeshObject>& mesh) -> void{
            get_mesh(id)->reset_rotation();
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_slice_factor(const MeshID id, const float level)
    {
        assert(0.0f <= level <= 1.0f);
        auto f = [level](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().slice_level = level;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_slice_lock(const MeshID id, const bool locked)
    {
        auto f = [locked](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().slice_locked = locked;
            if(locked)
            {
                mesh->just_locked = true;
            }

        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_peel_level(const MeshID id, const float level)
    {
        assert(level >= 0.0);
        auto f = [level](const std::shared_ptr<MeshObject>& mesh) -> void{
            if(level > mesh->get_data().max_peel_depth + 1.0f)
            {
                mesh->get_data().peel_level = mesh->get_data().max_peel_depth + 1.0f;
            }
            else
            {
                mesh->get_data().peel_level = level;
            }
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_reverse_peeling(const MeshID id, const bool reverse)
    {
        auto f = [reverse](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().reverse_peeling = reverse;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_cell_rounding(MeshID id, float rounding)
    {
        assert(0.0f <= rounding <= 1.0f);
        auto f = [rounding](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().rounding_size = rounding;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_cell_size(const MeshID id, const float size)
    {
        assert(0.0f <= size <= 1.0f);
        auto f = [size](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().cell_size = size;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_tessellation_level(const MeshID id, const int level)
    {
        assert(1 <= level && level <= 64);
        auto f = [level](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().tessellation_level = level;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_visibility(MeshID id, OpenVolumeMesh::CellHandle cell, bool visible)
    {
        auto f = [cell, visible](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_mvb()->set_cell_digged(cell.idx(), visible);
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_visibility(const MeshID id, const bool visible)
    {
        auto f = [visible](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().visible = visible;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::reset_visibility(MeshID id)
    {
        auto f = [](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_mvb()->reset_digging();
            mesh->get_mvb()->reset_isolation();
        };
        execute_for_mesh(f, id);
    }

    void MeshList::isolate(MeshID id, OpenVolumeMesh::CellHandle cell)
    {
        auto f = [cell](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_mvb()->set_cell_isolated(cell.idx());
        };
        execute_for_mesh(f, id);
    }

    void MeshList::hide(MeshID id, OpenVolumeMesh::CellHandle cell)
    {
        auto f = [cell](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_mvb()->set_cell_digged(cell.idx(), true);
        };
        execute_for_mesh(f, id);
    }

    void MeshList::never_discard(MeshID id, OpenVolumeMesh::CellHandle cell)
    {
        auto f = [cell](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_mvb()->set_cell_never_discard(cell.idx(), true);
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_name(MeshID id, const std::string& name)
    {
        auto f = [name](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().name = name;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_rendering_mode(MeshID id, RenderingMode mode)
    {
        auto f = [mode](const std::shared_ptr<MeshObject>& mesh) -> void{
            if(mode == RenderingMode::POINTS)
            {
                mesh->get_data().points = true;
            }
            else if(mode == RenderingMode::LINES)
            {
                mesh->get_data().lines = true;
            }
            else if(mode == RenderingMode::CELLS)
            {
                mesh->get_data().cells = true;
            }
        };
        execute_for_mesh(f, id);
    }

    void MeshList::render_cells(MeshID id, bool cells)
    {
        auto f = [cells](const std::shared_ptr<MeshObject>& mesh) -> void {
            mesh->get_data().cells = cells;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::render_lines(MeshID id, bool lines)
    {
        auto f = [lines](const std::shared_ptr<MeshObject>& mesh) -> void {
            mesh->get_data().lines = lines;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::render_points(MeshID id, bool points)
    {
        auto f = [points](const std::shared_ptr<MeshObject>& mesh) -> void {
            mesh->get_data().points = points;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_point_size(MeshID id, float size)
    {
        auto f = [size](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().point_size = size;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_line_width(MeshID id, float width)
    {
        auto f = [width](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().line_width = width;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_line_color(MeshID id, const glm::vec3& color)
    {
        auto f = [color](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().line_color = glm::vec4{color.r, color.g, color.b, 1.0f};
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_point_color(MeshID id, const glm::vec3& color)
    {
        auto f = [color](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().point_color = glm::vec4{color.r, color.g, color.b, 1.0f};
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_shading_mode(MeshID id, ShadingMode mode)
    {
        auto f = [mode](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().shading_mode = mode;
        };
        execute_for_mesh(f, id);
    }

    void volumeshOS::Internal::MeshList::set_origin(volumeshOS::Internal::MeshID id, const glm::vec3 &origin)
    {
        auto f = [origin](MeshID id, const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->set_origin(origin);
        };
        iterate(f);
    }

    void MeshList::set_color(const glm::vec4& color)
    {
        auto f = [color](MeshID id, const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->set_mesh_color(color);
        };
        iterate(f);
    }

    void MeshList::set_color(const MeshID id, const glm::vec4& color)
    {
        auto f = [color](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->set_mesh_color(color);
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_color(const MeshID id, OpenVolumeMesh::CellHandle cell, const glm::vec4& color)
    {
        auto f = [this, id, cell, color](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->set_cell_color(cell.idx(), color);
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_color(const MeshID id, OpenVolumeMesh::FaceHandle face, const glm::vec4& color)
    {
        auto f = [this, id, face, color](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->set_face_color(face.idx(), color);
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_color(const MeshID id, OpenVolumeMesh::HalfFaceHandle halfface, const glm::vec4& color)
    {
        auto f = [halfface, color](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->set_halfface_color(halfface.idx(), color);
        };
        execute_for_mesh(f, id);
    }

    /*
    void MeshList::set_color(const MeshID id, OpenVolumeMesh::EdgeHandle edge, const Color& color)
    {

    }

    void MeshList::set_color(const MeshID id, OpenVolumeMesh::VertexHandle vertex, const Color& color)
    {

    }
    */

    void MeshList::reset_selection(MeshID m_id)
    {
        auto f = [](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_mvb()->reset_selection();
        };
        execute_for_mesh(f, m_id);
    }

    void MeshList::select(EntityType type, MeshID m_id, HandleID h_id)
    {
        auto f = [type, h_id](const std::shared_ptr<MeshObject>& mesh) -> void{
            switch (type)
            {
                case EntityType::Halfface:
                    mesh->get_mvb()->set_halfface_selection(h_id, true);
                    break;
                case EntityType::Face:
                    mesh->get_mvb()->set_face_selection(h_id, true);
                    break;
                case EntityType::Cell:
                    mesh->get_mvb()->set_cell_selection(h_id, true);
                    break;
                default:
                    break;
            }
        };
        execute_for_mesh(f, m_id);
    }

    void MeshList::deselect(EntityType type, MeshID m_id, HandleID h_id)
    {
        auto f = [type, h_id](const std::shared_ptr<MeshObject>& mesh) -> void
        {
            switch (type)
            {
                case EntityType::Halfface:
                    mesh->get_mvb()->set_halfface_selection(h_id, false);
                    break;
                case EntityType::Face:
                    mesh->get_mvb()->set_face_selection(h_id, false);
                    break;
                case EntityType::Cell:
                    mesh->get_mvb()->set_cell_selection(h_id, false);
                    break;
                default:
                    break;
            }
        };
        execute_for_mesh(f, m_id);
    }

    bool MeshList::is_rendering_cells(MeshID id)
    {
        return get_mesh(id)->get_data().cells;
    }

    bool MeshList::is_rendering_lines(MeshID id)
    {
        return get_mesh(id)->get_data().lines;
    }

    bool MeshList::is_rendering_points(MeshID id)
    {
        return get_mesh(id)->get_data().points;
    }


    float MeshList::get_point_size(MeshID id)
    {
        return get_mesh(id)->get_data().point_size;
    }

    float MeshList::get_line_width(MeshID id)
    {
        return get_mesh(id)->get_data().line_width;
    }

    glm::vec3 MeshList::get_line_color(MeshID id)
    {
        auto color = get_mesh(id)->get_data().line_color;
        return glm::vec3{color.r, color.g, color.b} ;
    }

    glm::vec3 MeshList::get_point_color(MeshID id)
    {
        auto color = get_mesh(id)->get_data().point_color;
        return glm::vec3{color.r, color.g, color.b} ;
    }

    ShadingMode MeshList::get_shading_mode(MeshID id)
    {
        return get_mesh(id)->get_data().shading_mode;
    }

    float MeshList::get_ambient(const MeshID id)
    {
        return get_mesh(id)->get_data().ambient_strength;
    }

    float MeshList::get_diffuse(const MeshID id)
    {
        return get_mesh(id)->get_data().diffuse_strength;
    }

    float MeshList::get_specular(const MeshID id)
    {
        return get_mesh(id)->get_data().specular_strength;
    }

    float MeshList::get_specular_coefficient(const MeshID id)
    {
        return get_mesh(id)->get_data().specular_exponent;
    }

    const glm::vec3& MeshList::get_position(const MeshID id)
    {
        return get_mesh(id)->get_data().position;
    }

    float MeshList::get_scale(const MeshID id)
    {
        return get_mesh(id)->get_data().scale.x;
    }

    glm::vec3 MeshList::get_rotation(const MeshID id)
    {
        return get_mesh(id)->get_data().rotation_angles;
    }

    float MeshList::get_slice_factor(const MeshID id)
    {
        return get_mesh(id)->get_data().slice_level;
    }

    bool MeshList::get_slice_lock(const MeshID id)
    {
        return get_mesh(id)->get_data().slice_locked;
    }

    float MeshList::get_peel_level(const MeshID id)
    {
        return get_mesh(id)->get_data().peel_level;
    }

    bool MeshList::get_reverse_peeling(const MeshID id)
    {
        return get_mesh(id)->get_data().reverse_peeling;
    }

    int MeshList::get_max_peel_depth(const MeshID id)
    {
        return get_mesh(id)->get_max_peel_depth();
    }

    float MeshList::get_cell_rounding(const MeshID id)
    {
        return get_mesh(id)->get_data().rounding_size;
    }

    float MeshList::get_cell_size(const MeshID id)
    {
        return get_mesh(id)->get_data().cell_size;
    }

    int MeshList::get_tessellation_level(const MeshID id)
    {
//        if(get_mesh(id)->is_bezier_mesh())
//            return get_mesh(id)->get_data().tessellation_level;
//        else
//            return 0;
        return get_mesh(id)->get_data().tessellation_level;
    }

    bool MeshList::get_visibility(const MeshID id)
    {
        return get_mesh(id)->get_data().visible;
    }

    bool MeshList::get_visibility(const MeshID id, OpenVolumeMesh::CellHandle cell)
    {
        bool is_isolated = (bool) get_mesh(id)->get_mvb()->get_cell_isolate_value(cell.idx());
        if (is_isolated)
        {
            return true;
        }
        return not (bool) get_mesh(id)->get_mvb()->get_cell_dig_value(cell.idx());
    }

    const std::string& MeshList::get_name(MeshID id)
    {
        return get_mesh(id)->get_data().name;
    }

    glm::vec4 MeshList::get_color(const MeshID id)
    {
        return get_mesh(id)->get_data().color;
    }

    glm::vec4 MeshList::get_color(const MeshID id, OpenVolumeMesh::CellHandle cell)
    {
        return get_mesh(id)->get_mvb()->get_cell_color(cell.idx());
    }

    glm::vec4 MeshList::get_color(const MeshID id, OpenVolumeMesh::HalfFaceHandle halfface)
    {
        return get_mesh(id)->get_mvb()->get_halfface_color(halfface.idx());
    }

    glm::vec4 MeshList::get_color(const MeshID id, OpenVolumeMesh::EdgeHandle edge)
    {
        // TODO: Implement color for edges
        return glm::vec4{};
        // return get_mesh(id)->get_mvb()->get_cell_color(cell.idx());
    }

    glm::vec4 MeshList::get_color(const MeshID id, OpenVolumeMesh::VertexHandle vertex)
    {
        // TODO: Implement color for vertices
        return glm::vec4{};
        // return get_mesh(id)->get_mvb()->get_cell_color(cell.idx());
    }

    glm::vec3 MeshList::get_origin(const MeshID id)
    {
        return get_mesh(id)->get_data().origin;
    }

    bool MeshList::is_bezier_mesh(volumeshOS::Internal::MeshID id)
    {
        return get_mesh(id)->is_bezier_mesh();
    }


    void MeshList::execute_for_mesh(const std::function<void(std::shared_ptr<MeshObject>)>& func, MeshID id)
    {
        auto mesh = get_mesh(id);
        if( mesh != nullptr)
        {
            func(mesh);
        }
    }

    void MeshList::iterate(const std::function<void(MeshID id, std::shared_ptr<MeshObject>)>& func)
    {
        for(const auto& [id, mesh] : m_mesh_list)
        {
            if(mesh != nullptr)
            {
                func(id, mesh);
            }
        }
    }

//    OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* MeshList::load_from_file(const std::string& path)
//    {
//        OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d> ovm_mesh;
//        OpenVolumeMesh::IO::FileManager file_manager;
//        file_manager.readFile(path, ovm_mesh);
//        return ovm_mesh;
//    }

    void MeshList::calculate_selection_offsets()
    {
        int offset = 0;
        for(const auto& [id, mesh] : m_mesh_list)
        {
            mesh->set_selection_offset(offset);
            offset = std::get<1>(mesh->selection_offset()) + 1;
        }
    }

    std::shared_ptr<MeshObject> MeshList::get_mesh(MeshID id)
    {
        return m_mesh_list.find(id) != m_mesh_list.end() ? m_mesh_list[id] : nullptr;
    }


};
