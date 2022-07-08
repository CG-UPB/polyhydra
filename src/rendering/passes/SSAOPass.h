#pragma once

#include "RenderPass.h"
#include "../Renderer.h"
#include "../../settings/GlobalViewerSettings.h"

namespace volumeshOS::Internal
{
    class Renderer;
    class RenderPass;

    /**
     * Screen-Space Ambient Occlusion rendering pass.
     */
    class SSAOPass : public RenderPass
    {
    public:

        /**
         * Creates a new ambient occlusion pass, given a mesh view, width and height.
         *
         * @param mesh_view mesh view reference to access the pre pass framebuffer
         * @param initial_width framebuffer width
         * @param initial_height framebuffer height
         */
        SSAOPass(Renderer* renderer, int initial_width, int initial_height);
        ~SSAOPass();

        /**
         * Set the ambient occlusion options to new values.
         *
         * @param options new options
         */
        void load_options(const SSAOOptions& options);

        /**
         * Returns the currently used options of this ssao pass.
         *
         * @return current options
         */
        [[nodiscard]] const SSAOOptions& get_options() const;

        /**
         * Renders an ImGui window with the ambient occlusion options.
         *
         * @param options options to render
         */
        void load_options_from_settings();

        /**
         * Main ambient occlusion pass, this updates the occlusion factor texture that is used in the mesh pass.
         *
         * @param vao can be null, since we don't need it here
         * @param render_data current render data to access uniform data
         * @param mesh_id can be -1, since we don't need it here
         */
        void render(std::shared_ptr<VertexArrayObject> vao, const RenderData &render_data, std::shared_ptr<MeshObject> mesh) override;

        /**
         * Use this method to resize the ssao frame buffers in case the screen resolution changes.
         *
         * @param width new width
         * @param height new height
         */
        void resize_buffers(int width, int height);

        /**
         * Return the texture of the ssao main pass. However, this is not the final ambient occlusion factor.
         *
         * @return ssao texture
         */
        [[nodiscard]] unsigned int get_ssao_texture() const;

        /**
         * Return the texture of the ssao blur pass. This is the final occlusion factor.
         *
         * @return blur texture
         */
        [[nodiscard]] unsigned int get_blur_texture() const;

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
        unsigned int m_noise_texture;
    };
}