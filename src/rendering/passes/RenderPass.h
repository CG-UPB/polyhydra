#pragma once

class VertexArrayObject;

namespace vOS
{
    class RenderPass
    {
        virtual void render(const VertexArrayObject& vao) = 0;
    };
}
