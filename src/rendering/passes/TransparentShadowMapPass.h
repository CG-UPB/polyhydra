
#pragma once

#include "RenderPass.h"

namespace vOS
{

    class TransparentShadowMapPass : public RenderPass
    {

    public:

        TransparentShadowMapPass(int width, int height);
        ~TransparentShadowMapPass();

        void resize_buffers(int width, int height);
        void render(VertexArrayObject* vao, const RenderData& data, std::shared_ptr<MeshObject> mesh) override;

        FrameBufferObject* get_framebuffer() const;
        [[nodiscard]] unsigned int get_shadow_map() const;

    private:
        Shader* m_transparent_shadow_shader = nullptr;
        FrameBufferObject* m_transparent_shadow_framebuffer = nullptr;
    };
}
