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

        SSAOPass(MeshView* mesh_view, int initial_width, int initial_height);
        ~SSAOPass();

        void render(VertexArrayObject *vao, const RenderData &render_data, int mesh_id) override;

        void resize_buffers(int width, int height);

        [[nodiscard]] unsigned int get_ssao_texture() const;
        [[nodiscard]] unsigned int get_blur_texture() const;

    private:

        void generate_sample_kernel(std::uniform_real_distribution<float>& random_floats, std::default_random_engine& generator);
        void generate_noise_texture(std::uniform_real_distribution<float>& random_floats, std::default_random_engine& generator);

        // configuration
        static const int s_max_samples  = 64;

        bool m_ssao_active              = true;
        float m_sample_radius           = 0.5;
        float m_z_bias                  = 0.01;
        int m_num_samples               = 64;
        int m_strength                  = 1;
        int m_noise_size                = 4;

        // we keep a reference to access the pre-pass framebuffer
        MeshView* m_mesh_view;

        // rendering
        std::vector<glm::vec3> m_sample_kernel;
        FrameBufferObject* m_ssao_framebuffer;
        FrameBufferObject* m_blur_framebuffer;
        Shader* m_ssao_shader;
        Shader* m_ssao_blur_shader;
        unsigned int m_noise_texture;
    };
}