
#pragma once

#include <glm/vec3.hpp>
#include <OpenVolumeMesh/Core/GeometryKernel.hh>

namespace vOS
{
    typedef OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d> Mesh;
    typedef OpenVolumeMesh::VertexHandle Vertex;
    typedef OpenVolumeMesh::EdgeHandle Edge;

    class VecUtil
    {
    public:

        [[nodiscard]] static inline glm::vec3 pos_to_vec3(const Mesh& mesh, Vertex vertex_handle)
        {
            auto pos = mesh.vertex(vertex_handle);
            return {pos[0], pos[1], pos[2]};
        }

        [[nodiscard]] static inline glm::vec3 pos_to_vec3(const Mesh& mesh, int vertex_handle)
        {
            auto pos = mesh.vertex(OpenVolumeMesh::VertexHandle{vertex_handle});
            return {pos[0], pos[1], pos[2]};
        }

        [[nodiscard]] static inline glm::vec3 normal_to_vec3(const Mesh& mesh, OpenVolumeMesh::HalfFaceHandle halfface_handle)
        {
            auto normal = mesh.normal(halfface_handle);
            return {normal[0], normal[1], normal[2]};
        }

        [[nodiscard]] static inline glm::vec3 normal_to_vec3(const Mesh& mesh, int halfface_id)
        {
            auto normal = mesh.normal(OpenVolumeMesh::HalfFaceHandle{halfface_id});
            return {normal[0], normal[1], normal[2]};
        }

        [[nodiscard]] static inline std::array<glm::vec3, 2> edge_vertices(const Mesh& mesh, Edge edge_handle)
        {
            auto [v0, v1] = mesh.edge_vertices(edge_handle);
            return {pos_to_vec3(mesh, v0), pos_to_vec3(mesh, v1)};
        }

        [[nodiscard]] static inline float get_angle(const glm::vec3& vec0, const glm::vec3& vec1)
        {
            return glm::acos(glm::dot(vec0, vec1));
        }
    };
}
