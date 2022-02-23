
#pragma once

#include "RenderPass.h"

namespace vOS
{

    class ShadowMapPass : public RenderPass
    {

    public:

        ShadowMapPass(int width, int height);
        ~ShadowMapPass();

        void resize_buffers(int width, int height);
        void render(VertexArrayObject* vao, const RenderData& data, int mesh_id) override;

        FrameBufferObject* get_framebuffer() const;
        [[nodiscard]] unsigned int get_shadow_map() const;

    private:

        Shader* m_shadow_shader = nullptr;
        FrameBufferObject* m_shadow_framebuffer = nullptr;
        FrameBufferObject* m_shadow_framebuffer_transparent = nullptr;
    };
}
