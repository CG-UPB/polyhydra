#pragma once

#include <OpenVolumeMesh/Core/GeometryKernel.hh>
#include "../rendering/gl/VertexArrayObject.h"
#include "glm/gtx/transform.hpp"

namespace vOS
{
    typedef OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> Mesh;

    struct BufferSpecification
    {
        int peel_depth = 0;
    };

    class MeshVertexBuffer
    {
    public:
        explicit MeshVertexBuffer(Mesh* mesh, BufferSpecification spec);
        ~MeshVertexBuffer();

        unsigned int to_faceID(unsigned int value);
        unsigned int to_edgeID(unsigned int value);

        std::vector<float>& get_original_vertices();

        VertexArrayObject* get_vao();
        VertexArrayObject* get_sphere_vao();
        VertexArrayObject* get_cylinder_vao();

        [[nodiscard]] int get_num_visible_vertices() const;
        [[nodiscard]] int get_num_visible_edges() const;

    private:

        std::vector<float> get_vertices(Mesh& mesh);
        std::vector<unsigned int> get_indices(Mesh& mesh);
        std::vector<float> get_vertex_normals(Mesh& mesh);
        std::vector<unsigned int> get_edge_ids(Mesh& mesh, std::vector<unsigned int>& faces);
        std::vector<unsigned int> get_face_ids(Mesh& mesh);
        std::pair<std::vector<float>, std::vector<float>> get_from_and_to_vertices(Mesh& mesh, std::vector<unsigned int>& faces);

        void delete_boundary_cells(Mesh& mesh);

        BufferSpecification m_spec;

        std::vector<float> m_original_vertices;

        VertexArrayObject* m_vao;
        VertexArrayObject* m_sphere_vao;
        VertexArrayObject* m_cylinder_vao;

        std::vector<unsigned int> m_edge_ids;
        std::vector<unsigned int> m_face_ids;

        std::vector<float> m_vertices;
        std::vector<unsigned int> m_faces;
    };
}