#pragma once

#include "Shape.h"

namespace vOS
{
    class Sphere : public Shape
    {
    public:

        ~Sphere() override;
        void draw() override;

    private:

        VertexArrayObject* m_vao = nullptr;
    };
}