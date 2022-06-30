
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
        return ++m_id_count;
    }

    void MeshList::add_mesh(MeshID mesh_id, OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* mesh)
    {

        auto new_mesh = std::make_shared<MeshObject>(mesh_id);
        new_mesh->set_mesh_name(std::to_string(mesh->n_vertices()));
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
        auto ovm_mesh = load_from_file(path);
        add_mesh(mesh_id, ovm_mesh);
    }

    void MeshList::set_mesh(const MeshID id, OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* ovm_mesh)
    {
        auto f = [this, &id, &ovm_mesh](const std::shared_ptr<MeshObject>& mesh) -> void{
            auto mesh_obj = get_mesh(id);
            mesh->set_mesh(ovm_mesh);
        };

        execute_for_mesh(f, id);
    }

    void MeshList::set_mesh(const MeshID id, const std::string& path)
    {
        auto f = [this, &id, &path](const std::shared_ptr<MeshObject>& mesh) -> void{
            auto mesh_obj = get_mesh(id);
            auto ovm_mesh = load_from_file(path);
            mesh_obj->set_mesh(ovm_mesh);
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

    void MeshList::set_ambient(MeshID id, float ambient)
    {
        static auto f = [this, &ambient, &id](const std::shared_ptr<MeshObject>& mesh) -> void{
            get_mesh(id)->get_data().ambient_strength = ambient;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_diffuse(MeshID id, float diffuse)
    {
        static auto f = [this, &diffuse, &id](const std::shared_ptr<MeshObject>& mesh) -> void{
            get_mesh(id)->get_data().diffuse_strength = diffuse;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_specular(MeshID id, float specular)
    {
        static auto f = [this, &specular, &id](const std::shared_ptr<MeshObject>& mesh) -> void{
            get_mesh(id)->get_data().specular_strength = specular;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_specular_coefficient(MeshID id, float coefficient)
    {
        static auto f = [this, &coefficient, &id](const std::shared_ptr<MeshObject>& mesh) -> void{
            get_mesh(id)->get_data().specular_exponent = coefficient;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_phong(MeshID id, float ambient, float diffuse, float specular, float coefficient)
    {
        static auto f = [this, &ambient, &diffuse, &specular, &coefficient, &id](const std::shared_ptr<MeshObject>& mesh) -> void{
            get_mesh(id)->get_data().ambient_strength = ambient;
            get_mesh(id)->get_data().diffuse_strength = diffuse;
            get_mesh(id)->get_data().specular_strength = specular;
            get_mesh(id)->get_data().specular_exponent = coefficient;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_position(MeshID id, float x, float y, float z)
    {
        static auto f = [this, &x, &y, &z, &id](const std::shared_ptr<MeshObject>& mesh) -> void{
            //get_mesh(id)->get_data().position = glm::vec3(x, y, z);
            get_mesh(id)->translate(glm::vec3(x, y, z));
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_scale(MeshID id, float scale)
    {
        static auto f = [this, &scale, &id](const std::shared_ptr<MeshObject>& mesh) -> void{
            get_mesh(id)->scale(glm::vec3(scale));
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
        static auto f = [this, &level, &id](const std::shared_ptr<MeshObject>& mesh) -> void{
            get_mesh(id)->get_data().slice_level = level;
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
        static auto f = [this, &level, &id](const std::shared_ptr<MeshObject>& mesh) -> void{
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
        static auto f = [this, &size, &id](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().cell_size = size;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_visibility(MeshID id, OpenVolumeMesh::CellHandle cell, bool visible)
    {
        static auto f = [this, &cell, &visible](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_mvb()->set_cell_digged(cell.idx(), visible);
        };
        execute_for_mesh(f, id);
    }

    void MeshList::set_visibility(const MeshID id, const bool visible)
    {
        static auto f = [this, &visible, &id](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_data().visible = visible;
        };
        execute_for_mesh(f, id);
    }

    void MeshList::reset_visibility(MeshID id)
    {
        static auto f = [this](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_mvb()->reset_digging();
        };
        execute_for_mesh(f, id);
    }

    void MeshList::isolate(MeshID id, OpenVolumeMesh::CellHandle cell)
    {
        static auto f = [this, &cell](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_mvb()->set_cell_isolated(cell.idx());
        };
        execute_for_mesh(f, id);
    }

    void MeshList::hide(MeshID id, OpenVolumeMesh::CellHandle cell)
    {
        static auto f = [this, &cell](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->get_mvb()->set_cell_digged(cell.idx(), true);
        };
        execute_for_mesh(f, id);
    }


    void MeshList::set_color(const Color& color)
    {
        static auto f = [this, &color](MeshID id, const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->set_mesh_color(color);
        };
        iterate(f);
    }

    void MeshList::set_color(const MeshID id, const Color& color)
    {
        static auto f = [this, &id, &color](const std::shared_ptr<MeshObject>& mesh) -> void{
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
        static auto f = [this, &id, &halfface, &color](const std::shared_ptr<MeshObject>& mesh) -> void{
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
        if(auto mesh = get_mesh(m_id))
        {
            mesh->select_element(h_id, type);
        }
    }

    void MeshList::deselect(EntityType type, MeshID m_id, HandleID h_id)
    {
        if(auto mesh = get_mesh(m_id))
        {
            mesh->deselect_element(h_id, type);
        }
    }

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

    OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* MeshList::load_from_file(const std::string& path)
    {
        OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* ovm_mesh;
        OpenVolumeMesh::IO::FileManager file_manager;
        file_manager.readFile(path, *ovm_mesh);
        return ovm_mesh;
    }

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