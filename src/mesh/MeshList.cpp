
#include "MeshList.h"

namespace volumeshOS
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
        OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* ovm_mesh;
        OpenVolumeMesh::IO::FileManager file_manager;
        file_manager.readFile(path, *ovm_mesh);
        add_mesh(ovm_mesh);
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


    void MeshList::set_focused_mesh(const MeshID id)
    {
        auto f = [this, &id]() -> void{m_focused_mesh = id;};
        iterate(f, id);
    }

    std::shared_ptr<MeshObject> MeshList::get_focused_mesh(const MeshID id)
    {
        return get_mesh(m_focused_mesh);
    }

    void MeshList::set_visibility(const MeshID id, const bool visible)
    {
        static auto f = [this, &visible, &id]() -> void{ get_mesh(id)->get_data().visible = visible;};
        iterate(f, id);
    }

    void MeshList::set_peel_level(const MeshID id, const float level)
    {
        assert(0.0f <= level <= 1.0f);
        static auto f = [this, &level, &id]() -> void{ get_mesh(id)->get_data().peel_level = level;};
        iterate(f, id);
    }

    void MeshList::set_slice_level(const MeshID id, const float level)
    {
        assert(0.0f <= level <= 1.0f);
        static auto f = [this, &level, &id]() -> void{ get_mesh(id)->get_data().slice_level = level;};
        iterate(f, id);
    }

    void MeshList::set_slice_lock(const MeshID id, const bool locked)
    {
        static auto f = [this, &locked, &id]() -> void{ get_mesh(id)->get_data().slice_locked = locked;};
        iterate(f, id);
    }

    void MeshList::set_cell_size(const MeshID id, const float size)
    {
        assert(0.0f <= size <= 1.0f);
        static auto f = [this, &size, &id]() -> void{ get_mesh(id)->get_data().cell_size = size;};
        iterate(f, id);
    }

    void MeshList::set_color(EntityType type, MeshID m_id, HandleID h_id, Color color)
    {
        if(get_mesh(m_id))
        {
            switch (type)
            {
                case EntityType::Vertex:
                    break;
                case EntityType::Edge:
                    break;
                case EntityType::Face:
                    get_mesh(m_id)->set_face_color(h_id, color);
                    break;
                case EntityType::Cell:
                    get_mesh(m_id)->set_cell_color(h_id, color);
                    break;
                case EntityType::Mesh:
                    get_mesh(m_id)->set_mesh_color(color);
                    return;
            }
        }
    }

    void MeshList::select(EntityType type, MeshID m_id, HandleID h_id)
    {
        if(get_mesh(m_id))
        {
            switch (type)
            {
                case EntityType::Vertex:
                    //m_on_vertex_selection(mesh_id, element_handle_id, true);
                    break;
                case EntityType::Edge:
                    //m_on_edge_selection(mesh_id, element_handle_id, true);
                    break;
                case EntityType::Face:
                    //m_on_face_selection(mesh_id, element_handle_id, true);
                    break;
                case EntityType::Cell:
                    //m_on_cell_selection(mesh_id, element_handle_id, true);
                    break;
                case EntityType::Mesh:
                    //m_on_mesh_selection(mesh_id, element_handle_id, true);
                    return;
            }
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