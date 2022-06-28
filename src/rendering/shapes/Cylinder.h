#pragma once

#include "Shape.h"

namespace volumeshOS::Internal
{
    class Cylinder : public Shape
    {

    public:

        ~Cylinder() override;

        /**
         * draw method that overrides the draw-method of shape to draw a cylinder
         */
        void draw() override;

    private:
        // holds the vertices of the box
        VertexArrayObject* m_vao;
    };
}