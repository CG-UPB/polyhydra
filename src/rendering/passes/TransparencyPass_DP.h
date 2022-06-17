#pragma once

#include "RenderPass.h"
#include "../Renderer.h"

namespace vOS
{
    class Renderer;

    class TransparencyPass_DP : public RenderPass
    {
    public:
        explicit TransparencyPass_DP(Renderer* renderer, int width, int height);

        void render(std::shared_ptr<VertexArrayObject> vao, const RenderData& data, std::shared_ptr<MeshObject> mesh) override;
        void render(std::shared_ptr<VertexArrayObject> vao, const RenderData& data, std::shared_ptr<MeshObject> mesh, int pass);
        void render_composition(int current_passes, int max_passes);
        void resize_buffers(int width, int height);

        GLuint m_texture;

        unsigned int m_width;
        unsigned int m_height;

        std::shared_ptr<FrameBufferObject> m_transparent_framebuffer0;
        std::shared_ptr<FrameBufferObject> m_transparent_framebuffer1;

    private:
        void clean_up_framebuffer();

        void update_draw_texture();

        Renderer* m_renderer;
        std::shared_ptr<Shader> m_transparency_shader;
        std::shared_ptr<Shader> m_composite_shader;

        float m_alpha_pow = 1.0f;
        float m_pow = 1.0f;
        float m_range = 0.3f;
        float m_depth_range = 200.0f;
        float m_ordering_strength = 4.0f;
        float m_min = 0.01f;
        float m_max = 3000.0f;

        bool m_cullface = true;


    };
}