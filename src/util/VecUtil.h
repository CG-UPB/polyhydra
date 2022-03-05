
#pragma once

#include <glm/vec3.hpp>
#include <OpenVolumeMesh/Core/GeometryKernel.hh>

namespace vOS
{
    typedef OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d> Mesh;

    class VecUtil
    {
    public:

        [[nodiscard]] static inline glm::vec3 pos_to_vec3(const Mesh& mesh, OpenVolumeMesh::VertexHandle vertex_handle)
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

        [[nodiscard]] static inline std::array<glm::vec3, 2> edge_vertices(const Mesh& mesh, OpenVolumeMesh::EdgeHandle edge_handle)
        {
            auto [v0, v1] = mesh.edge_vertices(edge_handle);
            return {pos_to_vec3(mesh, v0), pos_to_vec3(mesh, v1)};
        }

        [[nodiscard]] static inline float get_angle(const glm::vec3& vec0, const glm::vec3& vec1)
        {
            return glm::acos(glm::dot(vec0, vec1));
        }

        [[nodiscard]] static std::pair<glm::vec3, glm::vec3> get_bounding_box(const std::vector<glm::vec3>& vertices)
        {
            glm::vec3 min = vertices[0];
            glm::vec3 max = vertices[0];
            for (int i = 1; i < vertices.size(); i++)
            {
                const glm::vec3& vertex = vertices[i];
                if (vertex.x < min.x)
                {
                    min.x = vertex.x;
                } else if (vertex.x > max.x)
                {
                    max.x = vertex.x;
                }
                if (vertex.y < min.y)
                {
                    min.y = vertex.y;
                } else if (vertex.y > max.y)
                {
                    max.y = vertex.y;
                }
                if (vertex.z < min.z)
                {
                    min.z = vertex.z;
                } else if (vertex.z > max.z)
                {
                    max.z = vertex.z;
                }
            }
            return std::make_pair(min, max);
        }

        [[nodiscard]] static inline glm::vec3 get_center(const std::vector<glm::vec3>& vertices)
        {
            auto bb = get_bounding_box(vertices);
            auto min = bb.first;
            auto max = bb.second;
            return min + (max - min) * 0.5f;
        }
    };
}
