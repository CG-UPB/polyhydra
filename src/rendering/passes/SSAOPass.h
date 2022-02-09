#pragma once

#include "../../panels/MeshView.h"
#include "RenderPass.h"
#include <random>

namespace vOS
{
    class MeshView;

    class SSAOPass : public RenderPass
    {
    public:

        static const int NUM_SAMPLES = 64;

        SSAOPass(MeshView* mesh_view, unsigned int initial_width, unsigned int initial_height);
        ~SSAOPass();

        void render(VertexArrayObject *vao, const RenderData &render_data, int mesh_id) override;

        void resize_buffers(unsigned int width, unsigned int height);

        [[nodiscard]] unsigned int get_ssao_texture() const;

    private:

        void generate_framebuffers(unsigned int width, unsigned int height);
        void clean_up_framebuffers();

        void generate_sample_kernel(std::uniform_real_distribution<float>& random_floats, std::default_random_engine& generator);
        void generate_noise_texture(std::uniform_real_distribution<float>& random_floats, std::default_random_engine& generator);

        unsigned int m_width;
        unsigned int m_height;

        unsigned int m_ssao_framebuffer;
        unsigned int m_ssao_texture;
        unsigned int m_blur_framebuffer;
        unsigned int m_blur_texture;
        unsigned int m_noise_texture;

        float m_uniform_radius = 0.5;
        float m_uniform_bias = 0.025;
        int m_uniform_strength = 1;
        int m_uniform_samples = 64;

        std::vector<glm::vec3> m_sample_kernel;

        MeshView* m_mesh_view = nullptr;
        Shader* m_ssao_shader = nullptr;
        Shader* m_ssao_blur_shader = nullptr;
        VertexArrayObject* m_screen_quad = nullptr;
    };
}