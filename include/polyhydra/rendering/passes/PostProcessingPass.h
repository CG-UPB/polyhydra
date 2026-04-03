#pragma once

#include "polyhydra/polyhydraPCH.h"
#include "polyhydra/rendering/passes/RenderPass.h"

namespace polyhydra::Internal
{
class PostProcessingPass : public RenderPass
{
  public:
    PostProcessingPass();
    ~PostProcessingPass() = default;

    void render(const Renderer& renderer) override;

  private:
    std::shared_ptr<Shader> m_post_processing_shader = nullptr;
};
} // namespace polyhydra::Internal