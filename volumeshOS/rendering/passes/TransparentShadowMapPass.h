
#pragma once

#include "RenderPass.h"

namespace vOS
{

    class TransparentShadowMapPass : public RenderPass
    {

    public:

        TransparentShadowMapPass(int width, int height);

        void resize_buffers(int width, int height);
        void render(std::shared_ptr<VertexArrayObject> vao, const RenderData& data, std::shared_ptr<MeshObject> mesh) override;

        [[nodiscard]] std::shared_ptr<FrameBufferObject> get_framebuffer() const;
        [[nodiscard]] unsigned int get_shadow_map() const;

    private:
        std::shared_ptr<Shader> m_transparent_shadow_shader;
        std::shared_ptr<FrameBufferObject> m_transparent_shadow_framebuffer;
    };
}
