#pragma once

#include "polyhydra/polyhydraPCH.h"

namespace polyhydra::Internal
{

class VecUtil
{
  public:
    typedef OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d> Mesh;

    static void print_mat(const glm::mat4& mat)
    {
        std::cout << "[";
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                std::cout << mat[i][j] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "]" << std::endl;
    }

    template <typename T>
    static inline void push_buffer(const std::vector<T>& src, std::vector<T>& dest)
    {
        dest.insert(dest.end(), src.begin(), src.end());
    }

    static inline void push_vec2(std::vector<float>& buffer, const glm::vec2& value)
    {
        buffer.push_back(value.x);
        buffer.push_back(value.y);
    }

    static inline void push_vec3(std::vector<float>& buffer, const glm::vec3& value)
    {
        buffer.push_back(value.x);
        buffer.push_back(value.y);
        buffer.push_back(value.z);
    }

    static inline void push_vec4(std::vector<float>& buffer, const glm::vec4& value)
    {
        buffer.push_back(value.x);
        buffer.push_back(value.y);
        buffer.push_back(value.z);
        buffer.push_back(value.w);
    }

    [[nodiscard]] static inline std::string to_string(const glm::vec3& vec)
    {
        return std::string("[" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z)
                           + "]");
    }

    [[nodiscard]] static inline glm::vec3 pos_to_vec3(const OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>& mesh,
                                                      OpenVolumeMesh::VertexHandle vertex_handle)
    {
        auto pos = mesh.vertex(vertex_handle);
        return {pos[0], pos[1], pos[2]};
    }

    [[nodiscard]] static inline glm::vec3 pos_to_vec3(const OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>& mesh,
                                                      int vertex_handle)
    {
        auto pos = mesh.vertex(OpenVolumeMesh::VertexHandle{vertex_handle});
        return {pos[0], pos[1], pos[2]};
    }

    [[nodiscard]] static inline glm::vec3
    normal_to_vec3(const OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>& mesh,
                   OpenVolumeMesh::HalfFaceHandle halfface_handle)
    {
        auto normal = mesh.normal(halfface_handle);
        return glm::normalize(glm::vec3{std::isnan(normal[0]) ? 0.0 : normal[0],
                                        std::isnan(normal[1]) ? 0.0 : normal[1],
                                        std::isnan(normal[2]) ? 0.0 : normal[2]});
    }

    [[nodiscard]] static inline glm::vec3
    normal_to_vec3(const OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>& mesh, int halfface_id)
    {
        auto normal = mesh.normal(OpenVolumeMesh::HalfFaceHandle{halfface_id});
        return glm::normalize(glm::vec3{std::isnan(normal[0]) ? 0.0 : normal[0],
                                        std::isnan(normal[1]) ? 0.0 : normal[1],
                                        std::isnan(normal[2]) ? 0.0 : normal[2]});
    }

    [[nodiscard]] static inline std::array<glm::vec3, 2>
    edge_vertices(const OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>& mesh,
                  OpenVolumeMesh::EdgeHandle edge_handle)
    {
        auto vs = mesh.edge_vertices(edge_handle);
        return {pos_to_vec3(mesh, vs[0]), pos_to_vec3(mesh, vs[1])};
    }

    [[nodiscard]] static inline float get_angle_fast(const glm::vec3& vec0, const glm::vec3& vec1)
    {
        float x = glm::dot(vec0, vec1);
        return glm::acos(glm::max(-1.0f, glm::min(x, 1.0f)));
    }

    [[nodiscard]] static inline float get_angle(const glm::vec3& vec0, const glm::vec3& vec1)
    {
        float x = glm::dot(vec0, vec1) / glm::length(vec0) * glm::length(vec1);
        return glm::acos(glm::max(-1.0f, glm::min(x, 1.0f)));
    }

    [[nodiscard]] static std::pair<glm::vec3, glm::vec3> get_bounding_box(const std::vector<glm::vec3>& vertices)
    {
        glm::vec3 min = vertices[0];
        glm::vec3 max = vertices[0];
        for (size_t i = 1; i < vertices.size(); i++)
        {
            const glm::vec3& vertex = vertices[i];
            if (vertex.x < min.x)
            {
                min.x = vertex.x;
            }
            else if (vertex.x > max.x)
            {
                max.x = vertex.x;
            }
            if (vertex.y < min.y)
            {
                min.y = vertex.y;
            }
            else if (vertex.y > max.y)
            {
                max.y = vertex.y;
            }
            if (vertex.z < min.z)
            {
                min.z = vertex.z;
            }
            else if (vertex.z > max.z)
            {
                max.z = vertex.z;
            }
        }
        return std::make_pair(min, max);
    }

    [[nodiscard]] static inline glm::vec3 get_bb_center(const std::vector<glm::vec3>& vertices)
    {
        auto bb = get_bounding_box(vertices);
        auto min = bb.first;
        auto max = bb.second;
        return min + (max - min) * 0.5f;
    }

    [[nodiscard]] static inline glm::vec3 get_center(const std::vector<glm::vec3>& vertices)
    {
        glm::vec3 average(0.0f);
        for (auto& vertex : vertices)
        {
            average += vertex;
        }
        return average /= (float)vertices.size();
    }
};
} // namespace polyhydra::Internal
