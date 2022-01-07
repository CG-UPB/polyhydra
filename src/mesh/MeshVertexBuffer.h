#pragma once

#include <OpenVolumeMesh/Core/GeometryKernel.hh>
#include "../rendering/gl/VertexArrayObject.h"
#include "glm/gtx/transform.hpp"

namespace vOS
{
    typedef OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> Mesh;
    typedef OpenVolumeMesh::CellHandle Cell;

    struct BufferSpecification
    {
        int peel_depth = 0;
    };

    struct VertexData
    {
        OpenVolumeMesh::VertexHandle ovm_handle;
        glm::vec3 position{};
        glm::vec3 normal{};
    };

    struct FaceData
    {
        std::vector<VertexData> vertices;
        std::vector<unsigned int> indices;
        std::vector<unsigned int> vertex_ids;
        std::vector<unsigned int> edge_ids;
        std::vector<unsigned int> face_ids;
    };

    class MeshVertexBuffer
    {
    public:
        explicit MeshVertexBuffer(Mesh* mesh, BufferSpecification spec);

        ~MeshVertexBuffer();

        int to_vertexID(int value);

        int to_edgeID(int value);

        int to_faceID(int value);

        std::vector<float>& get_original_vertices();

        VertexArrayObject* get_vao();

        VertexArrayObject* get_sphere_vao();

        VertexArrayObject* get_cylinder_vao();

        [[nodiscard]] int get_num_visible_vertices() const;

        [[nodiscard]] int get_num_visible_edges() const;

    private:

        void generate_buffer(Mesh& mesh);

        void add_cell(Mesh& mesh, Cell cell);

        void add_face_indices(Mesh& mesh, FaceData& face);

        void add_edge_id(Mesh& mesh, FaceData& face, int idx0, int idx1);

        void add_from_to_vertex(const VertexData& from, const VertexData& to);

        glm::vec3 get_center(const std::vector<glm::vec3>& vertices);

        std::vector<float> get_vertices(Mesh& mesh);

        void delete_boundary_cells(Mesh& mesh);

        BufferSpecification m_spec;

        std::vector<float> m_original_vertices;

        VertexArrayObject* m_vao = nullptr;
        VertexArrayObject* m_sphere_vao = nullptr;
        VertexArrayObject* m_cylinder_vao = nullptr;

        // ovm ids, in the order that we render them
        std::vector<int> m_vertex_ids;
        std::vector<int> m_edge_ids;
        std::vector<int> m_face_ids;

        // vertex attributes
        std::vector<float> m_positions;
        std::vector<float> m_normals;
        std::vector<float> m_cell_centers;

        std::vector<unsigned int> m_indices;
        std::vector<float> m_from_vertices;
        std::vector<float> m_to_vertices;

        int m_num_vertices = 0;
    };
}