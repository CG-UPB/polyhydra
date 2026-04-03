#pragma once

#include "polyhydra/polyhydraPCH.h"

#include "polyhydra/rendering/passes/RenderPass.h"

namespace polyhydra::Internal
{
class OutlinePass : public RenderPass
{
  public:
    OutlinePass();

    void render(const Renderer& renderer) override;

  private:
    std::shared_ptr<Shader> m_shader = nullptr;
};
} // namespace polyhydra::Internal
