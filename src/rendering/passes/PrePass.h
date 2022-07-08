
#pragma once

#include "RenderPass.h"
#include "../gl/PrePassFrameBufferObject.h"

namespace volumeshOS::Internal
{

    class PrePass : public RenderPass
    {

    public:

        PrePass(int width, int height);

        void resize_buffers(int width, int height);

        void render(const Renderer& renderer) override;

        [[nodiscard]] std::shared_ptr<PrePassFrameBufferObject> get_framebuffer() const;

    private:

        std::shared_ptr<Shader> m_clear_position_shader;
        std::shared_ptr<PrePassFrameBufferObject> m_pre_pass_framebuffer;
    };
}
