#pragma once

#include <glm/gtx/transform.hpp>
#include <OpenVolumeMesh/Core/GeometryKernel.hh>

namespace volumeshOS
{
    using MeshID = int;
    using HandleID = int;

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
        explicit Color(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f) : r(r), g(g), b(b), a(a)
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