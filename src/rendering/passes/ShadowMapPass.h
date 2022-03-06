
#pragma once

#include "RenderPass.h"
#include "../../panels/MeshView.h"


namespace vOS
{

    class ShadowMapPass : public RenderPass
    {

    public:

        ShadowMapPass(MeshView* mesh_view, int width, int height);
        ~ShadowMapPass();

        void resize_buffers(int width, int height);
        void render(VertexArrayObject* vao, const RenderData& data, int mesh_id) override;

        FrameBufferObject* get_framebuffer() const;
        [[nodiscard]] unsigned int get_shadow_map() const;

    private:

        MeshView* m_mesh_view;
        Shader* m_shadow_shader = nullptr;
        FrameBufferObject* m_shadow_framebuffer = nullptr;
    };
}
