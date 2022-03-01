
#pragma once

#include "RenderPass.h"

namespace vOS
{

    class TransparentShadowMapPass : public RenderPass
    {

    public:

        TransparentShadowMapPass(MeshView *mesh_view, int width, int height);
        ~TransparentShadowMapPass();

        void resize_buffers(int width, int height);
        void render(VertexArrayObject* vao, const RenderData& data, int mesh_id) override;

        FrameBufferObject* get_framebuffer() const;
        [[nodiscard]] unsigned int get_shadow_map() const;

    private:
        unsigned int m_width;
        unsigned int m_height;

        MeshView* m_mesh_view = nullptr;
        Shader* m_transparent_shadow_shader = nullptr;
        FrameBufferObject* m_transparent_shadow_framebuffer = nullptr;
    };
}
