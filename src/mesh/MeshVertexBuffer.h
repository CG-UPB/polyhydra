#pragma once

#include <OpenVolumeMesh/Core/GeometryKernel.hh>
#include "../rendering/gl/VertexArrayObject.h"
#include "glm/gtx/transform.hpp"

namespace vOS
{
    typedef OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d> Mesh;
    typedef OpenVolumeMesh::CellHandle Cell;

    struct BufferSpecification
    {
        int peel_depth = 0;
        int slice_depth = 0;
    };

    struct VertexData
    {
        glm::vec3 position{};
        glm::vec3 normal{};

        OpenVolumeMesh::VertexHandle ovm_handle;
    };

    struct FaceData
    {
        bool is_boundary = false;

        std::vector<VertexData> vertices;
        std::vector<unsigned int> indices;
        std::vector<unsigned int> face_ids;
    };

    class MeshVertexBuffer
    {
    public:
        explicit MeshVertexBuffer(Mesh *mesh, BufferSpecification spec);


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

        VertexArrayObject *get_vao();

        VertexArrayObject *get_sphere_vao();

        VertexArrayObject *get_cylinder_vao();


    private:
        /**
         * adds data to VertexBuffer for each cell
         * uses add_cell()
         * @param mesh
         */
        void generate_buffer(Mesh &mesh);

        void add_cell(Mesh &mesh, Cell cell);

        void add_face_indices(Mesh &mesh, FaceData &face) const;

        void add_from_to_vertex(Mesh &mesh, const OpenVolumeMesh::VertexHandle &from,
                                const OpenVolumeMesh::VertexHandle &to);

        static glm::vec3 get_center(const std::vector<glm::vec3> &vertices);

        static std::pair<glm::vec3, glm::vec3> get_bounding_box(const std::vector<glm::vec3> &vertices);

        static std::vector<float> get_vertices(Mesh &mesh);


        int m_num_vertices = 0;

        BufferSpecification m_spec;

        std::vector<float> m_original_vertices;

        VertexArrayObject *m_vao = nullptr;
        VertexArrayObject *m_sphere_vao = nullptr;
        VertexArrayObject *m_cylinder_vao = nullptr;

        // ovm ids, in the order that we render them
        std::vector<int> m_vertex_ids;
        std::vector<int> m_edge_ids;
        std::vector<int> m_face_ids;

        // vertex attributes
        std::vector<float> m_positions;
        std::vector<float> m_normals;
        std::vector<float> m_cell_centers;
        std::vector<float> m_is_face_boundary;
        std::vector<float> m_sphere_cell_centers;
        std::vector<float> m_cylinder_cell_centers;
        std::vector<float> m_sphere_peel_depths;
        std::vector<float> m_cylinder_peel_depths;
        std::vector<float> m_peel_depths;

        std::vector<unsigned int> m_indices;
        std::vector<float> m_from_vertices;
        std::vector<float> m_to_vertices;
        std::vector<float> m_selection_vertices;

    };
}