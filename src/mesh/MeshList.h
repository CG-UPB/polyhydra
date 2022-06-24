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

        /* Mesh Management */

        // add mesh by OVM-Mesh
        int add_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* mesh);

        // add mesh by path to OVM-FIle
        int add_mesh(const std::string& path);

        // deletes mesh
        void delete_mesh(MeshID id);

        // set current mesh (and focus camera on it)
        void set_focused_mesh(MeshID id);

        // get a specific mesh
        std::shared_ptr<MeshObject> get_focused_mesh(MeshID id);


        /* Mesh Setting */

        // set the position in world coordinate system
        void set_position();

        // set color for EntityType : a)Vertex, b)Edge, c) Face, d) Cell, e) whole Mesh
        void set_color(EntityType type, MeshID m_id, HandleID h_id, Color color);

        // set visibiliy of a specific mesh
        void set_visibility(MeshID id, bool visible);

        // set peel level of a specific mesh
        void set_peel_level(MeshID id, float level);

        // set slice level of a specific mesh
        void set_slice_level(MeshID id, float level);

        // lock slicing of a specific mesh
        void set_slice_lock(MeshID id, bool locked);

        // set cell size for a specific mesh
        void set_cell_size(MeshID id, float size);


        /* Selection */

        // set EntityType with its handle selected for a specific mesh
        void select(EntityType type, MeshID m_id, HandleID h_id);


        /* Utility */

        // iterate over meshes and execute func if id matches
        void iterate( const std::function<void()>&, MeshID id);

        // iterate over meshes and execute func for each mesh
        void iterate(const std::function<void(std::shared_ptr<MeshObject>)>& func);


    private:
        // calculate the range of selection-ids for each mesh
        void calculate_selection_offsets();

        // return mesh by id
        std::shared_ptr<MeshObject> get_mesh(MeshID id);

        // list of MeshObjects
        std::unordered_map<int, std::shared_ptr<MeshObject>> m_mesh_list;

        // number of total meshes
        int m_total_meshes;

        // id of the focused mesh
        MeshID m_focused_mesh;

    };

}