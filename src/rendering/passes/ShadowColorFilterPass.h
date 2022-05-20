#pragma once

#include "RenderPass.h"
#include "../Renderer.h"

namespace vOS
{
    class Renderer;

    class ShadowColorFilterPass : public RenderPass
    {

    public:

        ShadowColorFilterPass(Renderer* renderer, int width, int height);
        ~ShadowColorFilterPass();

        void resize_buffers(int width, int height);
        void render(VertexArrayObject* vao, const RenderData& data, std::shared_ptr<MeshObject> mesh) override;

        [[nodiscard]] FrameBufferObject* get_framebuffer() const;
        [[nodiscard]] unsigned int get_shadow_map() const;

    private:
        unsigned int m_width;
        unsigned int m_height;

        Renderer* m_renderer = nullptr;
        Shader* m_color_filter_shader = nullptr;
        FrameBufferObject* m_color_filter_framebuffer = nullptr;
    };
}
