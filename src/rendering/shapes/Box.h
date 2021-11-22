#pragma once

#include "Shape.h"

namespace vOS
{
    class Box : public Shape
    {
    public:

        explicit Box(float size_x = 1.0f, float size_y = 1.0f, float size_z = 1.0f);
        ~Box() override;
        void draw() override;

    private:

        glm::vec3 m_size;
        VertexArrayObject* m_vao = nullptr;
    };
}
