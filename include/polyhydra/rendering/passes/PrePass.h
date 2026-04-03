
#pragma once

#include "polyhydra/rendering/gl/PrePassFrameBufferObject.h"
#include "polyhydra/rendering/passes/RenderPass.h"

namespace polyhydra::Internal
{

class PrePass : public RenderPass
{

  public:
    PrePass(int width, int height);

    void resize_buffers(int width, int height);

    void render(const Renderer& renderer) override;

    [[nodiscard]] std::shared_ptr<PrePassFrameBufferObject> get_framebuffer() const;

    [[nodiscard]] uint32_t get_outline_texture() const;

  private:
    static void draw_with_shader(const Renderer& renderer, Shader& shader);

  private:
    std::shared_ptr<PrePassFrameBufferObject> m_pre_pass_framebuffer;
    std::shared_ptr<FrameBufferObject> m_outline_framebuffer;
    std::shared_ptr<Shader> m_outline_shader;
};
} // namespace polyhydra::Internal
