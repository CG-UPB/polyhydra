
#include "MeshList.h"

namespace volumeshOS::Internal
{
    MeshList::MeshList()
    {
        m_total_meshes = 0;
        m_focused_mesh = -1;
    }

    int MeshList::add_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* mesh)
    {
        int mesh_id = m_total_meshes++;
        auto new_mesh = std::make_shared<MeshObject>(mesh_id);
        new_mesh->set_mesh_name(std::to_string(mesh->n_vertices()));
        new_mesh->set_mesh(mesh);

        // add mesh and its id to our list
        m_mesh_list.emplace(mesh_id, std::move(new_mesh));

        // select mesh as currently focused (for settings and camera)
        set_focused_mesh(mesh_id);

        // calculate the range of the selection ids per mesh
        calculate_selection_offsets();

        return mesh_id;
    }

    int MeshList::add_mesh(const std::string& path)
    {
        auto ovm_mesh = load_from_file(path);
        add_mesh(ovm_mesh);
    }

    void MeshList::set_mesh(const MeshID id, OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* mesh)
    {
        auto f = [this, &id, &mesh]() -> void{
            auto mesh_obj = get_mesh(id);
            mesh_obj->set_mesh(mesh);
        };

        iterate(f, id);
    }

    void MeshList::set_mesh(const MeshID id, const std::string& path)
    {
        auto f = [this, &id, &path]() -> void{
            auto mesh_obj = get_mesh(id);
            auto ovm_mesh = load_from_file(path);
            mesh_obj->set_mesh(ovm_mesh);
        };

        iterate(f, id);
    }

    void MeshList::delete_mesh(const MeshID id)
    {
        for(auto it = m_mesh_list.begin(); it!= m_mesh_list.end();)
        {
            if(it->first == id)
            {
                m_mesh_list.erase(it);
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
    }


    void MeshList::set_focused_mesh(const MeshID id)
    {
        auto f = [this, &id]() -> void{
            m_focused_mesh = id;
        };
        iterate(f, id);
    }

    std::shared_ptr<MeshObject> MeshList::get_focused_mesh(const MeshID id)
    {
        return get_mesh(m_focused_mesh);
    }

    void MeshList::set_ambient(MeshID id, float ambient)
    {

    }

    void MeshList::set_diffuse(MeshID id, float diffuse)
    {

    }

    void MeshList::set_specular(MeshID id, float specular)
    {

    }

    void MeshList::set_specular_coefficient(MeshID id, float coefficient)
    {

    }

    void MeshList::set_phong(MeshID id, float ambient, float diffuse, float specular, float coefficient)
    {

    }

    void MeshList::set_position(MeshID id, float x, float y, float z)
    {

    }

    void MeshList::set_scale(MeshID id, float scale)
    {

    }

    void MeshList::set_rotation(MeshID id, float x, float y, float z)
    {

    }

    void MeshList::set_slice_factor(const MeshID id, const float level)
    {
        assert(0.0f <= level <= 1.0f);
        static auto f = [this, &level, &id]() -> void{
            get_mesh(id)->get_data().slice_level = level;
        };
        iterate(f, id);
    }

    void MeshList::set_slice_lock(const MeshID id, const bool locked)
    {
        static auto f = [this, &locked, &id]() -> void{
            get_mesh(id)->get_data().slice_locked = locked;
        };
        iterate(f, id);
    }

    void MeshList::set_peel_level(const MeshID id, const float level)
    {
        assert(0.0f <= level <= 1.0f);
        static auto f = [this, &level, &id]() -> void{
            get_mesh(id)->get_data().peel_level = level;
        };
        iterate(f, id);
    }

    void MeshList::set_cell_rounding(MeshID id, float rounding)
    {

    }

    void MeshList::set_cell_size(const MeshID id, const float size)
    {
        assert(0.0f <= size <= 1.0f);
        static auto f = [this, &size, &id]() -> void{
            get_mesh(id)->get_data().cell_size = size;
        };
        iterate(f, id);
    }

    void MeshList::set_visibility(MeshID id, OpenVolumeMesh::CellHandle cell, bool visible)
    {

    }

    void MeshList::set_visibility(const MeshID id, const bool visible)
    {
        static auto f = [this, &visible, &id]() -> void{
            get_mesh(id)->get_data().visible = visible;
        };
        iterate(f, id);
    }

    void MeshList::reset_visibility(MeshID id)
    {

    }

    void MeshList::isolate(MeshID id, OpenVolumeMesh::CellHandle cell)
    {

    }

    void MeshList::hide(MeshID id, OpenVolumeMesh::CellHandle cell)
    {

    }


    void MeshList::set_color(const Color& color)
    {
        static auto f = [this, &color](const std::shared_ptr<MeshObject>& mesh) -> void{
            mesh->set_mesh_color(color);
        };
        iterate(f);
    }

    void MeshList::set_color(const MeshID id, const Color& color)
    {
        static auto f = [this, &id, &color]() -> void{
            get_mesh(id)->set_mesh_color(color);
        };
        iterate(f, id);
    }

    void MeshList::set_color(const MeshID id, OpenVolumeMesh::CellHandle cell, const Color& color)
    {
        static auto f = [this, &id, &cell, &color]() -> void{
            get_mesh(id)->set_cell_color(cell.idx(), color);
        };
        iterate(f, id);
    }

    void MeshList::set_color(const MeshID id, OpenVolumeMesh::FaceHandle face, const Color& color)
    {
        static auto f = [this, &id, &face, &color]() -> void{
            get_mesh(id)->set_face_color(face.idx(), color);
        };
        iterate(f, id);
    }

    void MeshList::set_color(const MeshID id, OpenVolumeMesh::HalfFaceHandle halfface, const Color& color)
    {
        static auto f = [this, &id, &halfface, &color]() -> void{
            get_mesh(id)->set_face_color(halfface.idx(), color);
        };
        iterate(f, id);
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

    void MeshList::iterate(const std::function<void(std::shared_ptr<MeshObject>)>& func)
    {
        for(const auto& [id, mesh] : m_mesh_list)
        {
            func(mesh);
        }
    }

    void MeshList::iterate(const std::function<void()>& func, MeshID id)
    {
        for(auto it = m_mesh_list.begin(); it!= m_mesh_list.end();)
        {
            if(it->first == id)
            {
                func();
            }
            else
            {
                ++it;
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