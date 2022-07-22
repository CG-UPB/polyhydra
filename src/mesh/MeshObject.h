#pragma once

#include "vospch.h"

#include "../rendering/gl/VertexArrayObject.h"
#include "MeshVertexBuffer.h"
#include "../util/VecUtil.h"


namespace volumeshOS::Internal
{

    struct MeshData
    {

        [[nodiscard]] const glm::mat4& get_transform() const
        {
            return transformation;
        }

        void update_transform();

        glm::mat4 rotation          = glm::mat4(1.0f);
        glm::mat4 transformation    = glm::mat4(1.0f);

        // Rendering Variables
        Color color                 = Color{1.0f, 1.0f, 1.0f, 1.0f};
        Color selection_color       = Color{1.0f, 1.0f, 1.0f, 0.0f};
        float ambient_strength      = 1.0f;
        float diffuse_strength      = 1.0f;
        float specular_strength     = 0.3f;
        float specular_exponent     = 8.0f;

        // Toolbox Variables
        float peel_level            = 0.0f;
        int max_peel_depth          = 0;
        float slice_level           = 0.0f;
        float cell_size             = 1.0f;
        bool slice_locked           = false;
        bool rounding_active        = true;
        float rounding_size         = 0.2f;

        // Transform Variables
        glm::vec3 position          = {0.0f, 0.0f, 0.0f};
        glm::vec3 scale             = {1.0f, 1.0f, 1.0f};
        glm::vec3 position_offset   = {0.0f, 0.0f, 0.0f};
        float scale_normalization   = 1.0f;

        // Other
        bool visible                = true;
        int selection_id_offset     = 0;
        std::string name            = "default";
    };

    class MeshObject
    {
    public:

        explicit MeshObject(int id);

        // SelectionMode Functionality
        std::unordered_set<int>& get_all_selected_faces()
        {
            return m_selected_faces;
        }

        std::unordered_set<int>& get_all_selected_vertices()
        {
            return m_selected_vertices;
        }

        std::unordered_set<int>& get_all_selected_edges()
        {
            return m_selected_edges;
        }

        std::unordered_set<int>& get_all_selected_cells()
        {
            return m_selected_cells;
        }

        /**
         * Adds a shape on selected element (vertex, edge, face)
         * @param id ID to access element data
         * @param type declares type of element
         */
        void select_element(int id, EntityType type);

        /**
         * Removes a shape on selected element (vertex, edge, face)
         * @param id ID to access element data
         * @param type declares type of element
         */
        void deselect_element(int id, EntityType type);

        /**
         * Removes all shapes added by selection
         */
        void deselect_all();

        /**
         * Checks if a specific element is selected
         * @param id ID to access element data
         * @param type declares type of element
         * @return
         */
        bool is_element_selected(int id, EntityType type);

        void set_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d> *mesh);

        /**
         * Calculate the amount of needed selection ids
         * @param start id where ids start
         */
        void set_selection_offset(int start);

        void set_face_color(int ovm_id, Color color);

        void set_cell_color(int ovm_id, Color color);

        void set_mesh_color(Color color);

        /**
         * updates the vertex_buffer
         */
        void update_vertex_buffer();

        /**
         * converts selection id of vertices to OVM id
         * @param value id value
         * @return
         */
        int to_vertex_id(int value);

        /**
         * converts selection id of edges to OVM id
         * @param value id value
         * @return
         */
        int to_edge_id(int value);

        /**
         * converts selection id of faces to OVM id
         * @param value id value
         * @return
         */
        int to_halfface_id(int value);


        int get_max_peel_depth() const;

        const std::array<int, 2>& selection_offset()
        {
            return m_selection_offset;
        };

        glm::vec3& get_mesh_offset();

        [[nodiscard]] std::shared_ptr<VertexArrayObject> get_vao() const;

        glm::vec3 get_min();

        glm::vec3 get_max();

        std::pair<glm::vec3, glm::vec3> &get_world_bb(const glm::mat4 &transform);

        glm::vec3 &get_slice_dir(const glm::mat4 &world_transform, const glm::vec3 &view_dir);

        MeshData& get_data()
        {
            return m_data;
        }

        [[nodiscard]] std::shared_ptr<MeshVertexBuffer> get_mvb() const;

        [[nodiscard]] int get_num_visible_vertices() const;

        [[nodiscard]] int get_num_visible_edges() const;

        [[nodiscard]] std::shared_ptr<VertexArrayObject> get_cylinder_vao() const;

        [[nodiscard]] std::shared_ptr<VertexArrayObject> get_sphere_vao() const;

        [[nodiscard]] int get_id() const
        {
            return m_id;
        }

        [[nodiscard]] std::shared_ptr<OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>> get_ovm() const;

        void translate(const glm::vec3& vec);

        void scale(const glm::vec3& vec);

        void rotate(float angle, const glm::vec3& axis);

        void reset_rotation();

    private:
        /**
         * Gets the center of the mesh (e.g for rotation) by calculating the bounding_box
         */
        void calculate_mesh_offset();

        /**
         * Calculates the depth of vertices and cells
         */
        void calculate_peel_depth();

        /**
         * calculates the amount of needed ids
         * @return
         */
        [[nodiscard]] int calculate_selection_size() const;

        std::shared_ptr<OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>> m_mesh;

        const int key_multiplier = 1000000;

        bool m_just_locked;

        std::unordered_set<int> m_selected_vertices;
        std::unordered_set<int> m_selected_edges;
        std::unordered_set<int> m_selected_faces;
        std::unordered_set<int> m_selected_cells;

        std::map<int, int> m_created_shapes;

        std::array<int, 2> m_selection_offset;

        std::pair<glm::vec3, glm::vec3> m_transformed_bb;

        glm::vec3 m_mesh_offset_from_center;

        glm::vec3 m_slice_dir;

        std::shared_ptr<MeshVertexBuffer> m_mvb = nullptr;

        MeshData m_data;

        int m_id;
    };
}
