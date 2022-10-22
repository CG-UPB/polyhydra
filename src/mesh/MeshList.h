#pragma once

#include "vospch.h"
#include "MeshObject.h"
#include "../util/Types.h"

namespace volumeshOS::Internal
{
    class MeshList
    {
    public:
        MeshList();

        /* Mesh Management */

        // add mesh by OVM-Mesh
        void add_mesh(MeshID id, const std::shared_ptr<OVMesh>& mesh);

        // add mesh by path to OVM-FIle
        void add_mesh(MeshID id, const std::string& path);

        // sets OVM-Mesh to existing ID
        void set_mesh(MeshID id, const std::shared_ptr<OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>>& ovm_mesh);

        // sets OVM-Mesh by path to existing ID
        void set_mesh(MeshID id, const std::string& path);

        // deletes mesh
        void delete_mesh(MeshID id);

        // deletes mesh
        void delete_meshes();

        // return mesh by id
        std::shared_ptr<MeshObject> get_mesh(MeshID id);

        // set current mesh (and focus camera on it)
        void set_focused_mesh(MeshID id);

        // get a specific mesh
        std::shared_ptr<MeshObject> get_focused_mesh();

        // get ID of focused mesh
        MeshID get_focused_mesh_id();

        /* SelectionMode */

        void reset_selection(MeshID m_id);

        // set EntityType with its handle selected for a specific mesh
        void select(EntityType type, MeshID m_id, HandleID h_id);

        // set EntityType with its handle unselected for a specific mesh
        void deselect(EntityType type, MeshID m_id, HandleID h_id);


        /* Mesh Setting */

        // set the position in world coordinate system
        void set_position();

        // Set color for all meshes (all cells and halffaces)
        void set_color(const glm::vec4& color);

        // Set color for one mesh (all cells and halffaces)
        void set_color(MeshID id, const glm::vec4& color);

        // Set color for one cell of a mesh
        void set_color(MeshID id, OpenVolumeMesh::CellHandle cell, const glm::vec4& color);

        // Set color for one face (both halffaces) of a mesh
        void set_color(MeshID id, OpenVolumeMesh::FaceHandle face, const glm::vec4& color);

        // Set color for one halfface of a mesh
        void set_color(MeshID id, OpenVolumeMesh::HalfFaceHandle halfface, const glm::vec4& color);

        // Set color for one edge of a mesh
        void set_color(MeshID id, OpenVolumeMesh::EdgeHandle edge, const glm::vec4& color);

        // Set color for one vertex of a mesh
        void set_color(MeshID id, OpenVolumeMesh::VertexHandle vertex, const glm::vec4& color);

        // Set the ambient term for the phong lighting model of a mesh
        void set_ambient(MeshID id, float ambient);

        // Set the diffuse term for the phong lighting model of a mesh
        void set_diffuse(MeshID id, float diffuse);

        // Set the specular term for the phong lighting model of a mesh
        void set_specular(MeshID id, float specular);

        // Set the specular coefficient for the phong lighting model of a mesh
        void set_specular_coefficient(MeshID id, float coefficient);

        // Set parameters for the phong lighting model of a mesh
        void set_phong(MeshID id, float ambient, float diffuse, float specular, float coefficient);

        // Set the position of a mesh
        void set_position(MeshID id, float x, float y, float z);

        // Set the scale of a mesh
        void set_scale(MeshID id, float scale);

        // Set the rotation of a mesh using euler angles
        void set_rotation(MeshID id, float x, float y, float z);

        // Resets rotation
        void reset_rotation(MeshID id);

        // Set the slice factor for a mesh. 0 (no slicing) to 1 (full slicing of the mesh)
        void set_slice_factor(MeshID id, float factor);

        // Lock the direction of the slice plane
        void set_slice_lock(MeshID id, bool lock);

        // Set the peel level for a given mesh. 0 (no peel) up to the total number of depth layers in the mesh
        void set_peel_level(MeshID id, float level);

        // Toggle reverse peeling
        void set_reverse_peeling(MeshID id, bool reverse);

        // Set the rounding factor for each cell of a mesh. 0 (no rounding) to 1 (full rounding)
        void set_cell_rounding(MeshID id, float rounding);

        // Set the cell size of a given mesh. 0 (infinitely small) to 1 (original size)
        void set_cell_size(MeshID id, float size);

        // Set the tessellation level of a given mesh (1 - 64). (non-Bézier meshes are not affected)
        void set_tessellation_level(MeshID id, int level);

        // Set the cell visibility of a mesh
        void set_visibility(MeshID id, OpenVolumeMesh::CellHandle cell, bool visible);

        // Set the visibility of a mesh
        void set_visibility(MeshID id, bool visible);

        // Reset the visibility so that all cells are visible
        void reset_visibility(MeshID id);

        // Isolate a single cell of a mesh, making it the only visible cell
        void isolate(MeshID id, OpenVolumeMesh::CellHandle cell);

        // Hide a cell of a mesh
        void hide(MeshID id, OpenVolumeMesh::CellHandle cell);

        // Set the name of a mesh
        void set_name(MeshID id, const std::string& name);

        void set_tesselation_level(MeshID id, const int level);

        int get_tesselation_level(MeshID id);


        /* Getter */

        // Returns the ambient term for the phong lighting model of a mesh
        float get_ambient(MeshID id);

        // Returns the diffuse term for the phong lighting model of a mesh
        float get_diffuse(MeshID id);

        // Returns the specular term for the phong lighting model of a mesh
        float get_specular(MeshID id);

        // Returns the specular coefficient for the phong lighting model of a mesh
        float get_specular_coefficient(MeshID id);

        // Returns the position of a mesh
        const glm::vec3& get_position(MeshID id);

        // Returns the scale of a mesh
        float get_scale(MeshID id);

        // Returns the rotation of a mesh using euler angles
        glm::vec3 get_rotation(MeshID id);

        // Returns the slice factor for a mesh. 0 (no slicing) to 1 (full slicing of the mesh)
        float get_slice_factor(MeshID id);

        // Returns the lock direction of the slice plane
        bool get_slice_lock(MeshID id);

        // Returns the peel level for a given mesh. 0 (no peel) up to the total number of depth layers in the mesh
        float get_peel_level(MeshID id);

        // Returns if reverse peeling is active
        float get_reverse_peeling(MeshID id);

        // Returns the number of cell depth layers of the mesh
        int get_max_peel_depth(MeshID id);

        // Activate Roundings
        void activate_rounding(MeshID id, bool rounding);

        // Returns the rounding factor for each cell of a mesh. 0 (no rounding) to 1 (full rounding)
        float get_cell_rounding(MeshID id);

        // Returns the cell size of a given mesh. 0 (infinitely small) to 1 (original size)
        float get_cell_size(MeshID id);

        // Returns the tessellation level of a given mesh. 0 is returned for non-Bézier meshes
        int get_tessellation_level(MeshID id);

        // Returns the visibility of a cell
        bool get_visibility(MeshID id, OpenVolumeMesh::CellHandle cell);

        // Returns the visibility of the mesh
        bool get_visibility(MeshID id);

        // Returns the name of a mesh
        const std::string& get_name(MeshID id);

        // Returns the color of the mesh
        glm::vec4 get_color(MeshID id);

        // Returns the color of a cell from a given mesh
        glm::vec4 get_color(MeshID id, OpenVolumeMesh::CellHandle cell);

        // Returns the color of a halfface from a given mesh
        glm::vec4 get_color(MeshID id, OpenVolumeMesh::HalfFaceHandle halfface);

        // Returns the color of an edge from a given mesh
        glm::vec4 get_color(MeshID id, OpenVolumeMesh::EdgeHandle edge);

        // Returns the color of a vertex from a given mesh
        glm::vec4 get_color(MeshID id, OpenVolumeMesh::VertexHandle vertex);

        /* Utility */

        // iterate over meshes and execute func if id matches
        void execute_for_mesh( const std::function<void(std::shared_ptr<MeshObject>)>& func, MeshID id);

        // iterate over meshes and execute func for each mesh
        void iterate(const std::function<void(MeshID ,std::shared_ptr<MeshObject>)>& func);


    private:
        // calculate the range of selection-ids for each mesh
        void calculate_selection_offsets();

        // update OVM-Mesh from file
//        static OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>* load_from_file(const std::string& path);

        // list of MeshObjects
        std::unordered_map<int, std::shared_ptr<MeshObject>> m_mesh_list;

        // number of total meshes
        int m_total_meshes;

        // id of the focused mesh
        MeshID m_focused_mesh;

    };

}