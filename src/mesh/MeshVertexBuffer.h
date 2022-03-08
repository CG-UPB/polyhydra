#pragma once

#include <OpenVolumeMesh/Core/GeometryKernel.hh>
#include "../rendering/gl/VertexArrayObject.h"
#include "glm/gtx/transform.hpp"
#include <map>
#include <unordered_map>

namespace vOS
{
    typedef OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d> Mesh;
    typedef OpenVolumeMesh::CellHandle Cell;

    struct VertexData
    {
        glm::vec3 position{};
        glm::vec3 normal{};
        glm::vec3 color{};
    };

    struct FaceData
    {
        std::vector<VertexData> vertices;
        std::vector<unsigned int> indices;
        std::vector<unsigned int> face_ids;
    };

    struct RoundedVertexData
    {
        int from_vertex_id = -1;
        int to_vertex_id = -1;
        int halfedge_id = -1;
        int halfface_id = -1;
    };

    struct RoundedFaceVertexData
    {
        unsigned int index = -1;
        int corner_vertex_id = -1;
        int to_vertex_id = -1;
        int next_to_vertex_id = -1;
        int to_vertex_halfedge_id = -1;
        int next_to_vertex_halfedge_id = -1;
    };

    struct RoundedCellData
    {
        int cell_id = -1;
        std::vector<float> vertex_types;
        std::vector<float> vertex_positions;
        std::vector<float> vertex_normals;
        std::vector<float> vertex_cell_centers;
        std::vector<float> vertex_colors;
        std::vector<float> vertex_peel_depths;
        std::vector<float> vertex_is_triangle;
        std::vector<float> vertex_is_digged;
        std::vector<float> vertex_is_isolated;
        std::vector<float> face_center_or_to_vertex;
        std::vector<unsigned int> indices;
    };

    class MeshVertexBuffer
    {
    public:

        explicit MeshVertexBuffer(Mesh *mesh);
        ~MeshVertexBuffer();

        /**
         * converts selection id of vertices to OVM id
         * @param value id value
         * @return
         */
        int to_vertexID(int value);

        /**
         * converts selection id of edges to OVM id
         * @param value id value
         * @return
         */
        int to_edgeID(int value);

        /**
         * converts selection id of faces to OVM id
         * @param value id value
         * @return
         */
         int to_faceID(int value);

        [[nodiscard]] int get_num_selection_vertices() const;

        [[nodiscard]] int get_num_selection_edges() const;

        std::vector<float> &get_original_vertices();

        VertexArrayObject *get_vao_by_face();

        VertexArrayObject* get_vao_rounded();

        [[nodiscard]] float get_average_cell_size() const;

        VertexArrayObject *get_vao_transparent_by_face();

        VertexArrayObject* get_vao_transparent_rounded();

        /**
         * Colors the given Face in desired colors
         * The strong a is, the more pronounced the given color is
         * An Alpha value of 0 will have no visible effect, 1 will completely override the object default color
         * @param ovm_id
         * @param r
         * @param g
         * @param b
         * @param a
         */
        void set_face_color(int ovm_id, float r, float g, float b, float a);

        /**
         * Select or unselect the given Face
         * @param ovm_id
         * @param selected
         */
        void set_face_selection(int ovm_id, bool selected);

        VertexArrayObject* get_sphere_vao();

        VertexArrayObject* get_cylinder_vao();

        void update_digging_buffer(int id, float newValue);

        void update_isolate_buffer(int id, float newValue);

        void reset_isolation();

        void reset_digging();

        void activate_isolation();

        void start_isolation();

    private:

        static constexpr float ROUNDED_VERTEX_TYPE_FACE     = 0.0f;
        static constexpr float ROUNDED_VERTEX_TYPE_EDGE     = 1.0f;
        static constexpr float ROUNDED_VERTEX_TYPE_CORNER   = 2.0f;
        static constexpr float ROUNDED_VERTEX_TYPE_CENTER   = 3.0f;

        /**
         * adds data to VertexBuffer for each cell
         * uses add_cell()
         * @param mesh
         */
        void generate_buffer(Mesh &mesh);

        void add_cell_rounded(Mesh& mesh, Cell cell);

        unsigned int add_vertex_data_to_cell_data(RoundedCellData& data, float type, const glm::vec3& pos, const glm::vec3& norm, const glm::vec4& col, const glm::vec3& fc_or_tv, float angle);

        void add_cell_triangle_indices(RoundedCellData& data, unsigned int i0, unsigned int i1, unsigned int i2);

        void add_cell_by_faces(Mesh& mesh, Cell cell);

        void add_face_indices(Mesh &mesh, FaceData &face) const;

        void add_from_to_vertex(Mesh &mesh, const OpenVolumeMesh::VertexHandle &from,
                                const OpenVolumeMesh::VertexHandle &to);

        static std::vector<float> get_vertices(Mesh &mesh);

        /**
         * Set to true, if some update has been made to the vao buffers (like face color)
         * To reduce overhead we do not update the vao immediatly, but only before it has been requested by some other class
         */
        bool m_update_vao = false;
        int m_face_amount = 0;
        int m_cell_start_face_index = 0;
        float m_average_cell_size;

        std::vector<float> m_original_vertices;

        VertexArrayObject* m_vao_by_face = nullptr;
        VertexArrayObject* m_vao_rounded = nullptr;
        VertexArrayObject* m_vao_transparent_by_face = nullptr;
        VertexArrayObject* m_vao_transparent_rounded = nullptr;
        VertexArrayObject* m_sphere_vao = nullptr;
        VertexArrayObject* m_cylinder_vao = nullptr;

        // ovm ids, in the order that we render them
        std::vector<int> m_selection_vertices_ids;
        std::vector<int> m_selection_edges_ids;
        std::vector<int> m_selection_halffaces_ids;

        // to be used for rounded cells as well, no need to calculate twice
        std::unordered_map<int, glm::vec3> m_cell_centers;
        std::unordered_map<int, float> m_peel_depths;

        // vertex attributes for cells by face
        std::vector<float> m_positions_by_face;
        std::vector<float> m_normals_by_face;
        std::vector<float> m_cell_centers_by_face;
        std::vector<float> m_colors_by_face;
        std::vector<float> m_selections;
        std::vector<float> m_peel_depths_by_face;
        std::vector<float> m_is_triangle_by_face;
        std::vector<float> m_is_digged_by_face;
        std::vector<float> m_is_isolated_by_face;

        // vertex attributes for rounded cells
        std::vector<float> m_positions_rounded;
        std::vector<float> m_normals_rounded;
        std::vector<float> m_cell_centers_rounded;
        std::vector<float> m_colors_rounded;
        std::vector<float> m_peel_depths_rounded;
        std::vector<float> m_is_triangle_rounded;
        std::vector<float> m_is_digged_rounded;
        std::vector<float> m_is_isolated_rounded;
        std::vector<float> m_vertex_types_rounded;
        std::vector<float> m_face_center_or_to_vertex_rounded;
        int m_current_rounded_index = 0;

        // selection
        std::vector<float> m_sphere_cell_centers;
        std::vector<float> m_cylinder_cell_centers;
        std::vector<float> m_sphere_peel_depths;
        std::vector<float> m_cylinder_peel_depths;

        std::vector<float> m_sphere_is_digged;
        std::vector<float> m_cylinder_is_digged;

        std::vector<float> m_sphere_is_isolated;
        std::vector<float> m_cylinder_is_isolated;

        std::vector<unsigned int> m_indices;
        std::vector<unsigned int> m_indices_rounded;
        std::vector<float> m_from_vertices;
        std::vector<float> m_to_vertices;
        std::vector<float> m_selection_vertices;

        /**
         * Maps OVM Ids to face buffer locations
         */
        std::map<int,int> m_ovm_to_gl_face_indizes;
        std::vector<int> m_face_offset_array;
        std::vector<int> m_face_vertex_count;
        int m_total_vertex_count = 0;

        std::map<int, int> m_start_of_cell_vertices;

        std::map<int, int> m_size_of_cell_vertices;
        std::map<int, int> m_selection_sphere_digging_indices;
        std::map<int, int> m_selection_cylinder_digging_indices;
        std::map<int, int> m_selection_sphere_digging_numbers;
        std::map<int, int> m_selection_cylinder_digging_numbers;

        int m_num_vertices = 0;
    };
}