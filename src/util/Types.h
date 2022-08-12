#pragma once

#include <glm/gtx/transform.hpp>
#include <OpenVolumeMesh/Core/GeometryKernel.hh>
#include <type_traits>
#include <cassert>

namespace volumeshOS
{
    namespace Internal
    {
        using MeshID    = int;
        using ShapeID   = int;
        using HandleID  = int;

        typedef OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>   OVMesh;
        typedef OpenVolumeMesh::VertexHandle                            OVMVertex;
        typedef OpenVolumeMesh::EdgeHandle                              OVMEdge;
        typedef OpenVolumeMesh::HalfEdgeHandle                          OVMHalfEdge;
        typedef OpenVolumeMesh::FaceHandle                              OVMFace;
        typedef OpenVolumeMesh::HalfFaceHandle                          OVMHalfFace;
        typedef OpenVolumeMesh::CellHandle                              OVMCell;

        static constexpr const MeshID INVALID_MESH_ID                   = -1;
        static constexpr const ShapeID INVALID_SHAPE_ID                 = -1;

        template<typename Vec3T>
        [[nodiscard]] static glm::vec3 to_glm_vec3(const Vec3T& vecType)
        {
            if constexpr (std::is_same_v<Vec3T, glm::vec3>)
            {
                return vecType;
            }
            else if constexpr (std::is_same_v<Vec3T, OpenVolumeMesh::Vec3d>)
            {
                auto val = static_cast<OpenVolumeMesh::Vec3d>(vecType);
                return glm::vec3{val[0], val[1], val[2]};
            }
            else if constexpr (std::is_same_v<Vec3T, OpenVolumeMesh::Vec3f>)
            {
                auto val = static_cast<OpenVolumeMesh::Vec3f>(vecType);
                return glm::vec3{val[0], val[1], val[2]};
            }
            else if constexpr (std::is_same_v<Vec3T, std::array<double, 3>>)
            {
                auto val = static_cast<std::array<double, 3>>(vecType);
                return glm::vec3{val[0], val[1], val[2]};
            }
            else if constexpr (std::is_same_v<Vec3T, std::array<float, 3>>)
            {
                auto val = static_cast<std::array<float, 3>>(vecType);
                return glm::vec3{val[0], val[1], val[2]};
            }
            assert(false && "Invalid type conversion to vec3");
        }

        template<typename Vec3T>
        [[nodiscard]] static Vec3T glm_vec3_to(const glm::vec3& vecType)
        {
            if constexpr (std::is_same_v<Vec3T, glm::vec3>)
            {
                return vecType;
            }
            else if constexpr (std::is_same_v<Vec3T, OpenVolumeMesh::Vec3d>)
            {
                return OpenVolumeMesh::Vec3d{vecType.x, vecType.y, vecType.z};
            }
            else if constexpr (std::is_same_v<Vec3T, OpenVolumeMesh::Vec3f>)
            {
                return OpenVolumeMesh::Vec3f{vecType.x, vecType.y, vecType.z};
            }
            else if constexpr (std::is_same_v<Vec3T, std::array<double, 3>>)
            {
                return std::array<double, 3>{vecType.x, vecType.y, vecType.z};
            }
            else if constexpr (std::is_same_v<Vec3T, std::array<float, 3>>)
            {
                return std::array<float, 3>{vecType.x, vecType.y, vecType.z};
            }
            assert(false && "Invalid type conversion from vec3");
        }

        template<typename Vec4T>
        [[nodiscard]] static glm::vec4 to_glm_vec4(const Vec4T& vecType)
        {
            if constexpr (std::is_same_v<Vec4T, glm::vec4>)
            {
                return vecType;
            }
            else if constexpr (std::is_same_v<Vec4T, OpenVolumeMesh::Vec4d>)
            {
                auto val = static_cast<OpenVolumeMesh::Vec4d>(vecType);
                return glm::vec4{val[0], val[1], val[2], val[3]};
            }
            else if constexpr (std::is_same_v<Vec4T, OpenVolumeMesh::Vec4f>)
            {
                auto val = static_cast<OpenVolumeMesh::Vec4f>(vecType);
                return glm::vec4{val[0], val[1], val[2], val[3]};
            }
            else if constexpr (std::is_same_v<Vec4T, std::array<double, 4>>)
            {
                auto val = static_cast<std::array<double, 4>>(vecType);
                return glm::vec4{val[0], val[1], val[2], val[3]};
            }
            else if constexpr (std::is_same_v<Vec4T, std::array<float, 4>>)
            {
                auto val = static_cast<std::array<float, 4>>(vecType);
                return glm::vec4{val[0], val[1], val[2], val[3]};
            }
            assert(false && "Invalid type conversion to vec4");
        }

        template<typename Vec4T>
        [[nodiscard]] static Vec4T glm_vec4_to(const glm::vec4& vecType)
        {
            if constexpr (std::is_same_v<Vec4T, glm::vec4>)
            {
                return vecType;
            }
            else if constexpr (std::is_same_v<Vec4T, OpenVolumeMesh::Vec4d>)
            {
                return OpenVolumeMesh::Vec4d{vecType.x, vecType.y, vecType.z, vecType.w};
            }
            else if constexpr (std::is_same_v<Vec4T, OpenVolumeMesh::Vec4f>)
            {
                return OpenVolumeMesh::Vec4f{vecType.x, vecType.y, vecType.z, vecType.w};
            }
            else if constexpr (std::is_same_v<Vec4T, std::array<double, 4>>)
            {
                return std::array<double, 4>{vecType.x, vecType.y, vecType.z, vecType.w};
            }
            else if constexpr (std::is_same_v<Vec4T, std::array<float, 4>>)
            {
                return std::array<float, 4>{vecType.x, vecType.y, vecType.z, vecType.w};
            }
            assert(false && "Invalid type conversion from vec4");
        }
    }

    enum class EntityType
    {
        Vertex,
        Edge,
        Halfface,
        Face,
        Cell,
        Mesh,
        All
    };

    enum class Theme
    {
        Light,
        Dark
    };

}