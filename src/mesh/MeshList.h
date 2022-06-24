#pragma once

#include "vospch.h"
#include "MeshObject.h"


namespace volumeshOS
{
    using MeshID = int;
    using HandleID = int;


    class MeshList
    {
    public:
        MeshList();

        // Mesh Management
        //MeshObject* get_mesh(MeshID);
        int add_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* mesh);
        int add_mesh(const std::string& path);
        void delete_mesh(MeshID id);
        void set_focused_mesh(MeshID id);
        std::shared_ptr<MeshObject> get_focused_mesh(MeshID id);

        // Mesh Setting
        void set_position();
        void set_color(EntityType type, Color color, MeshID id);
        void select(EntityType type, HandleID h_id, MeshID m_id);
        void set_visibility(MeshID id, bool visible);
        void set_peel_level(MeshID id, float level);
        void set_slice_level(MeshID id, float level);
        void set_slice_lock(MeshID id, bool locked);
        void set_cell_size(MeshID id, float size);

        // Selection
        // ...

        void iterate( const std::function<void()>&, MeshID id);
        //void iterate( void (*func)(std::shared_ptr<MeshObject>));


    private:
        void calculate_selection_offsets();
        std::shared_ptr<MeshObject> get_mesh(MeshID id);

        std::unordered_map<int, std::shared_ptr<MeshObject>> m_mesh_list;
        int m_total_meshes;
        int m_focused_mesh;

    };

}