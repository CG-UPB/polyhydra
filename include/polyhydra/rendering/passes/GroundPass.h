#pragma once

#include "polyhydra/rendering/passes/RenderPass.h"

namespace polyhydra::Internal
{
/**
 * Renders the Background visible when no other Mesh occupies the Viewfield
 */
class GroundPass : public RenderPass
{
  public:
    GroundPass();

    void render(const Renderer& renderer) override;

    void render_pre(const Renderer& renderer);

  private:
    std::shared_ptr<VertexArrayObject> m_vao = nullptr;
    std::shared_ptr<Shader> m_ground_shader = nullptr;
    std::shared_ptr<Shader> m_pre_ground_shader = nullptr;
};
} // namespace polyhydra::Internal