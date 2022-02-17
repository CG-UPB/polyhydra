#pragma once

#include "Shape.h"

namespace vOS
{
    class Sphere : public Shape
    {
    public:

        ~Sphere() override;
        /**
         * draw method that overrides the draw-method of shape to draw a sphere
         */
        void draw() override;

    private:
        // holds the vertices of the box
        VertexArrayObject* m_vao = nullptr;
    };
}