#pragma once

#include "RenderPass.h"
#include "../Renderer.h"
#include "../../settings/AppState.h"

namespace volumeshOS::Internal
{
    /**
     * Screen-Space Ambient Occlusion rendering pass.
     */
    class SSAOPass : public RenderPass
    {
    public:

        SSAOPass(int initial_width, int initial_height);
        ~SSAOPass();

        void load_options(const SSAOOptions& options);
        [[nodiscard]] const SSAOOptions& get_options() const;

        void load_options_from_settings();

        void render(const Renderer& renderer) override;

        void resize_buffers(int width, int height);

        [[nodiscard]] uint32_t get_ssao_texture() const;

        [[nodiscard]] uint32_t get_blur_texture() const;

        // different presets to choose from
        static const SSAOOptions QUALITY_SSAO;
        static const SSAOOptions BALANCED_SSAO;
        static const SSAOOptions PERFORMANCE_SSAO;

    private:

        /**
         * Generates a random float between min and max.
         *
         * @param min minimum value
         * @param max maximum value
         * @return random float
         */
        [[nodiscard]] float get_random_float(float min, float max) const;

        /**
         * Linearly interpolates between two values based on a factor.
         *
         * @param a min value
         * @param b max value
         * @param factor interpolation factor between 0 and 1
         * @return interpolated value
         */
        [[nodiscard]] float lerp(float a, float b, float factor) const;

        /**
         * Generates a random sample kernel used to sample the screen depth.
         */
        void generate_sample_kernel();

        /**
         * Generates a random noise texture to randomize the kernel rotation in the shader.
         */
        void generate_noise_texture();

        // configuration
        static const int s_max_samples = 64;
        static const int s_noise_size = 4;

        float m_dot;

        // options
        SSAOOptions m_options;
        int m_selected_preset = 1;

        // we keep a reference to access the pre-pass framebuffer
        Renderer* m_renderer;

        // rendering
        std::vector<glm::vec3> m_sample_kernel;
        std::shared_ptr<FrameBufferObject> m_ssao_framebuffer;
        std::shared_ptr<FrameBufferObject> m_blur_framebuffer;
        std::shared_ptr<Shader> m_ssao_shader;
        std::shared_ptr<Shader> m_ssao_blur_shader;
        uint32_t m_noise_texture;
    };
}