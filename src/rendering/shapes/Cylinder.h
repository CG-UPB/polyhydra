#pragma once

#include "Shape.h"

namespace vOS
{
    class Cylinder : public Shape
    {

    public:

        ~Cylinder() override;
        void draw() override;

    private:

        VertexArrayObject* m_vao;
    };
}