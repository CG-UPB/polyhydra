#pragma once

#include "polyhydra/rendering/passes/RenderPass.h"

namespace polyhydra::Internal
{

class BackgroundPass : public RenderPass
{
  public:
    BackgroundPass();

    void render(const Renderer& renderer) override;

  private:
    std::shared_ptr<Shader> m_background_shader = nullptr;
};
} // namespace polyhydra::Internal