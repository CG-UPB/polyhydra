#pragma once

#include "../Renderer.h"
#include "RenderPass.h"

namespace vOS
{
    class Renderer;

    class TransparencyPass_DP : public RenderPass
    {
    public:
        explicit TransparencyPass_DP(Renderer* renderer, int width, int height);
        ~TransparencyPass_DP();

        void render(VertexArrayObject* vao, const RenderData& data, int mesh_id) override;
        void render(VertexArrayObject* vao, const RenderData& data, int mesh_id, int pass);
        void render_composition(int current_passes, int max_passes);
        void resize_buffers(int width, int height);
        void clear_framebuffer() const;

        GLuint m_texture;

        unsigned int m_width;
        unsigned int m_height;

        FrameBufferObject* m_transparent_framebuffer0 = nullptr;
        FrameBufferObject* m_transparent_framebuffer1 = nullptr;

    private:
        void clean_up_framebuffer();

        void update_draw_texture();

        Renderer* m_renderer = nullptr;
        Shader* m_transparency_shader = nullptr;
        Shader* m_composite_shader = nullptr;
        VertexArrayObject* m_vao;

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