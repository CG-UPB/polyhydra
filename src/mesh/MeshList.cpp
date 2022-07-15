
#include "MeshList.h"

namespace volumeshOS::Internal
{
    MeshList::MeshList()
    {
        m_total_meshes = 0;
        m_id_count = 0;
        m_focused_mesh = -1;
    }

    MeshID MeshList::next_id()
    {
        return m_id_count++;
    }

    void MeshList::add_mesh(MeshID mesh_id, OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* mesh)
    {

        auto new_mesh = std::make_shared<MeshObject>(mesh_id);
        //int x = mesh->n_vertices();
        new_mesh->set_mesh_name("test");
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
        OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d> ovm_mesh;
        OpenVolumeMesh::IO::FileManager file_manager;
        file_manager.readFile(path, ovm_mesh);
        add_mesh(mesh_id, &ovm_mesh);
    }

    void MeshList::set_mesh(const MeshID id, OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* ovm_mesh)
    {
        auto f = [&ovm_mesh](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->set_mesh(ovm_mesh);
        };

        execute_for_mesh(f, id);
    }

    void MeshList::set_mesh(const MeshID id, const std::string& path)
    {
        auto f = [&path](const std::shared_ptr<MeshObject>& mesh) -> void{
            OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d> ovm_mesh;
            OpenVolumeMesh::IO::FileManager file_manager;
            file_manager.readFile(path, ovm_mesh);
            mesh->set_mesh(&ovm_mesh);
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
        auto f = [this, &id](const std::shared_ptr<MeshObject>& mesh) -> void{
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
        if (auto mesh = get_mesh(m_focused_mesh))
        {
            return mesh->get_id();
        }
        return -1;
    }

    void MeshList::set_ambient(MeshID id, float ambient)
    {
        static auto f = [&ambient](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().ambient_strength = ambient;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_diffuse(MeshID id, float diffuse)
    {
        static auto f = [&diffuse](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().diffuse_strength = diffuse;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_specular(MeshID id, float specular)
    {
        static auto f = [&specular](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().specular_strength = specular;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_specular_coefficient(MeshID id, float coefficient)
    {
        static auto f = [&coefficient](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().specular_exponent = coefficient;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_phong(MeshID id, float ambient, float diffuse, float specular, float coefficient)
    {
        static auto f = [&ambient, &diffuse, &specular, &coefficient](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().ambient_strength = ambient;
            mesh->get_data().diffuse_strength = diffuse;
            mesh->get_data().specular_strength = specular;
            mesh->get_data().specular_exponent = coefficient;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_position(MeshID id, float x, float y, float z)
    {
        static auto f = [&x, &y, &z](const std::shared_ptr<MeshObject>& mesh) -> void{
            //get_mesh(id)->get_data().position = glm::vec3(x, y, z);
            mesh->translate(glm::vec3(x, y, z));
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_scale(MeshID id, float scale)
    {
        static auto f = [&scale](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->scale(glm::vec3(scale));
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_rotation(MeshID id, float x, float y, float z)
    {
        /*
        static auto f = [this, &x, &y, &z, &id](const std::shared_ptr<MeshObject>& mesh) -> void{
            get_mesh(id)->rotate(glm::vec3(x, y, z));
        };
        execute_for_mesh(f, id);
        */
    }

    void MeshList::set_slice_factor(const MeshID id, const float level)
    {
        assert(0.0f <= level <= 1.0f);
        static auto f = [&level](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().slice_level = level;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_slice_lock(const MeshID id, const bool locked)
    {
        static auto f = [&locked](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().slice_locked = locked;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_peel_level(const MeshID id, const float level)
    {
        assert(0.0f <= level <= 1.0f);
        static auto f = [&level](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().peel_level = level;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_cell_rounding(MeshID id, float rounding)
    {
        static auto f = [&rounding](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().rounding_size = rounding;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_cell_size(const MeshID id, const float size)
    {
        assert(0.0f <= size <= 1.0f);
        static auto f = [&size](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().cell_size = size;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_visibility(MeshID id, OpenVolumeMesh::CellHandle cell, bool visible)
    {
        static auto f = [&cell, &visible](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_mvb()->set_cell_digged(cell.idx(), visible);
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_visibility(const MeshID id, const bool visible)
    {
        static auto f = [&visible](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().visible = visible;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::reset_visibility(MeshID id)
    {
        static auto f = [](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_mvb()->reset_digging();
        };
        execute_for_mesh(f, id);
    }

    void MeshList::isolate(MeshID id, OpenVolumeMesh::CellHandle cell)
    {
        static auto f = [&cell](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_mvb()->set_cell_isolated(cell.idx());
        };
        execute_for_mesh(f, id);
    }

    void MeshList::hide(MeshID id, OpenVolumeMesh::CellHandle cell)
    {
        static auto f = [&cell](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_mvb()->set_cell_digged(cell.idx(), true);
        };
        execute_for_mesh(f, id);
    }


    void MeshList::set_color(const Color& color)
    {
        static auto f = [&color](MeshID id, const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->set_mesh_color(color);
        };
        iterate(f);
    }

    void MeshList::set_color(const MeshID id, const Color& color)
    {
        static auto f = [&color](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->set_mesh_color(color);
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_color(const MeshID id, OpenVolumeMesh::CellHandle cell, const Color& color)
    {
        static auto f = [this, &id, &cell, &color](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->set_cell_color(cell.idx(), color);
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_color(const MeshID id, OpenVolumeMesh::FaceHandle face, const Color& color)
    {
        static auto f = [this, &id, &face, &color](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->set_face_color(face.idx(), color);
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_color(const MeshID id, OpenVolumeMesh::HalfFaceHandle halfface, const Color& color)
    {
        static auto f = [&halfface, &color](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->set_face_color(halfface.idx(), color);
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

    void MeshList::select(EntityType type, MeshID m_id, HandleID h_id)
    {
        auto f = [&type, &h_id](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->select_element(h_id, type);
        };
        execute_for_mesh(f, m_id);
    }

    void MeshList::deselect(EntityType type, MeshID m_id, HandleID h_id)
    {
        auto f = [&type, &h_id](const std::shared_ptr<MeshObject>& mesh) -> void
        {
            mesh->deselect_element(h_id, type);
        };
        execute_for_mesh(f, m_id);
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

    std::array<float, 3> MeshList::get_position(const MeshID id)
    {
        auto p = get_mesh(id)->get_data().position;
        std::array<float, 3> pos = {p[0], p[1], p[2]};
        return pos;
    }

    std::array<float, 3> MeshList::get_scale(const MeshID id)
    {
        auto s = get_mesh(id)->get_data().scale;
        std::array<float, 3> scl = {s[0], s[1], s[2]};
        return scl;
    }

    std::array<float, 3> MeshList::get_rotation(const MeshID id)
    {
        auto r = get_mesh(id)->get_data().rotation;
        float yaw, pitch, roll = 0.0f;

        pitch = -asin(r[0][2]);
        yaw = asin(r[1][2] / cos(pitch));
        roll = acos(r[0][0] / cos(roll));

        std::array<float, 3> rot = {yaw, pitch, roll};
        return rot;
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

    int MeshList::get_max_peel_depth(const MeshID id)
    {
        return get_mesh(id)->get_data().peel_level;
    }

    float MeshList::get_cell_rounding(const MeshID id)
    {
        return get_mesh(id)->get_data().rounding_size;
    }

    float MeshList::get_cell_size(const MeshID id)
    {
        return get_mesh(id)->get_data().cell_size;
    }

    bool MeshList::get_visibility(const MeshID id)
    {
        return get_mesh(id)->get_data().visible;
    }

    bool MeshList::get_visibility(const MeshID id, OpenVolumeMesh::CellHandle cell)
    {
        return get_mesh(id)->is_element_selected(cell.idx(), EntityType::Cell);
    }

    Color MeshList::get_color(const MeshID id)
    {
        return get_mesh(id)->get_data().color;
    }

    Color MeshList::get_color(const MeshID id, OpenVolumeMesh::CellHandle cell)
    {
        return get_mesh(id)->get_mvb()->get_cell_color(cell.idx());
    }

    Color MeshList::get_color(const MeshID id, OpenVolumeMesh::HalfFaceHandle halfface)
    {
        return get_mesh(id)->get_mvb()->get_halfface_color(halfface.idx());
    }

    /*
    Color MeshList::get_color(const MeshID id, OpenVolumeMesh::EdgeHandle edge)
    {
        return get_mesh(id)->get_mvb()->get_cell_color(cell.idx());
    }

    Color MeshList::get_color(const MeshID id, OpenVolumeMesh::VertexHandle vertex)
    {
        return get_mesh(id)->get_mvb()->get_cell_color(cell.idx());
    }
    */


    void MeshList::execute_for_mesh(const std::function<void(std::shared_ptr<MeshObject>)>& func, MeshID id)
    {
        if(auto mesh = get_mesh(id))
        {
            func(mesh);
        }
    }

    void MeshList::iterate(const std::function<void(MeshID id, std::shared_ptr<MeshObject>)>& func)
    {
        for(const auto& [id, mesh] : m_mesh_list)
        {
            if(mesh)
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