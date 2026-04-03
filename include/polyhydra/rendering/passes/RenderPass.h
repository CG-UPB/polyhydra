#pragma once

namespace polyhydra::Internal
{
class Renderer;
class Shader;
class VertexArrayObject;

struct RenderPass
{
    virtual void render(const Renderer& renderer) = 0;
};
} // namespace polyhydra::Internal
