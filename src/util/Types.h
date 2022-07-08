#pragma once

#include <glm/gtx/transform.hpp>
#include <OpenVolumeMesh/Core/GeometryKernel.hh>

namespace volumeshOS
{
    namespace Internal
    {
        typedef OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>   OVMesh;
        typedef OpenVolumeMesh::VertexHandle                            OVMVertex;
        typedef OpenVolumeMesh::EdgeHandle                              OVMEdge;
        typedef OpenVolumeMesh::HalfEdgeHandle                          OVMHalfEdge;
        typedef OpenVolumeMesh::FaceHandle                              OVMFace;
        typedef OpenVolumeMesh::HalfFaceHandle                          OVMHalfFace;
        typedef OpenVolumeMesh::CellHandle                              OVMCell;
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

    enum Mode
    {
        FLY,
        ORBIT
    };

    struct Color
    {
        Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a)
        {}

        Color(const glm::vec3& color) : Color(color.r, color.g, color.b)
        {}

        Color(const glm::vec4& color) : Color(color.r, color.g, color.b, color.a)
        {}

        [[nodiscard]] glm::vec3 get_rgb() const
        {
            return glm::vec3{r, g, b};
        }

        [[nodiscard]] glm::vec4 get_rgba() const
        {
            return glm::vec4{r, g, b, a};
        }

        float r, g, b, a;
    };

}