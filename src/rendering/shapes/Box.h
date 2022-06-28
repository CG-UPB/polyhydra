#pragma once

#include "Shape.h"

namespace volumeshOS::Internal
{
    /**
     * This class represents a Box as subclass of Shape
     */
    class Box : public Shape
    {
    public:
        /**
         * Constructor that in default mode creates a box with size 1*1*1
         *
         * @param size_x
         * @param size_y
         * @param size_z
         */
        explicit Box(float size_x = 1.0f, float size_y = 1.0f, float size_z = 1.0f);
        ~Box() override;

        /**
         * draw method that overrides the draw-method of shape to draw a box
         */
        void draw() override;

    private:

        glm::vec3 m_size;
        // holds the vertices of the box
        VertexArrayObject* m_vao = nullptr;
    };
}
