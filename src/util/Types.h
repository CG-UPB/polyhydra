#pragma once

#include <glm/gtx/transform.hpp>
#include <OpenVolumeMesh/Core/GeometryKernel.hh>

namespace volumeshOS
{
    class Mesh
    {
    public:
        explicit Mesh(int id);
        void set_color()
        {
            volumeshOS::set_color(m_id, ...);
        }

    private:

        int m_id;
    };


    Mesh mesh = volumeshOS::add_mesh(...);

    volumeshOS::add_mesh(...);

    volumeshOS::set_color(mesh, ...);
    volumeshOS::set_color(...);

    volumeshOS::set_color(mesh.get_id(), ...);


    mesh.set_color(...);



    enum class EntityType
    {
        Vertex,
        Edge,
        Face,
        Cell,
        Mesh
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
        explicit Color(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f) : rgba(r, g, b, a)
        {}

        [[nodiscard]] glm::vec3 get_rgb() const
        {
            return glm::vec3{rgba};
        }

        [[nodiscard]] const glm::vec4& get_rgba() const
        {
            return rgba;
        }

    private:
        glm::vec4 rgba;
    };

}