#pragma once

#include "../Renderer.h"
#include "RenderPass.h"

namespace vOS
{
    class Renderer;

    class TransparencyPass_WB : public RenderPass
    {
    public:
        explicit TransparencyPass_WB(Renderer* renderer, int width, int height);
        ~TransparencyPass_WB();

        void render(VertexArrayObject* vao, const RenderData& data, int mesh_id) override;
        void render_composition();
        void resize_buffers(int width, int height);
        void clear_framebuffer() const;

        GLuint m_reveal_texture;
        GLuint m_accum_texture;
        GLuint m_depth_texture;

        void generate_transparency_framebuffer(int width, int height);
        void bind_transparent_buffer();
        void unbind_transparent_buffer();

        unsigned int get_accum_texture();
        unsigned int get_reveal_texture();

    private:
        void clean_up_framebuffer();

        glm::vec4 m_zeros = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        glm::vec4 m_ones =glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

        Renderer* m_renderer = nullptr;

        Shader* m_transparency_shader = nullptr;
        Shader* m_composite_shader = nullptr;

        FrameBufferObject* m_transparent_framebuffer = nullptr;

        float m_alpha_pow = 1.0f;
        float m_pow = 1.0f;
        float m_range = 0.3f;
        float m_depth_range = 200.0f;
        float m_ordering_strength = 4.0f;
        float m_min = 0.01f;
        float m_max = 3000.0f;

    };
}