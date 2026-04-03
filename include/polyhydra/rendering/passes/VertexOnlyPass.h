#pragma once

#include "polyhydra/rendering/gl/Shader.h"
#include "polyhydra/rendering/passes/RenderPass.h"

namespace polyhydra::Internal
{

class VertexOnlyPass : public RenderPass
{

  public:
    VertexOnlyPass();
    ~VertexOnlyPass() = default;

    void render(const Renderer& renderer) override;

  private:
    std::shared_ptr<Shader> m_vertex_only_shader = nullptr;
};

} // namespace polyhydra::Internal
