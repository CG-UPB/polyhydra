#pragma once

#include "RenderPass.h"
#include "../Renderer.h"
#include "rendering/gl/Shader.h"

namespace volumeshOS::Internal
{
    class Renderer;

    class TransparencyPassWB : public RenderPass
    {
    public:
        explicit TransparencyPassWB(const Renderer& renderer, int width, int height);

        void render(const Renderer& renderer) override;
        void render_composition();
        void resize_buffers(const Renderer& renderer,int width, int height);
        void clear_framebuffer() const;

        GLuint m_reveal_texture;
        GLuint m_accum_texture;
        GLuint m_depth_texture;

        void generate_transparency_framebuffer(const Renderer& renderer);

        uint32_t get_accum_texture();
        uint32_t get_reveal_texture();

    private:
        void clean_up_framebuffer();

        glm::vec4 m_zeros = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        glm::vec4 m_ones =glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

        Renderer* m_renderer;
        std::shared_ptr<Shader> m_transparency_shader;
        std::shared_ptr<Shader> m_composite_shader;
        std::shared_ptr<FrameBufferObject> m_transparent_framebuffer;

        float m_alpha_pow = 1.0f;
        float m_pow = 1.0f;
        float m_range = 0.3f;
        float m_depth_range = 200.0f;
        float m_ordering_strength = 4.0f;
        float m_min = 0.01f;
        float m_max = 3000.0f;

    };
}