#pragma once

#include "RenderPass.h"
#include "../../panels/MeshView.h"
#include "rendering/gl/Shader.h"

namespace volumeshOS::Internal
{

    class ShadowMapPass : public RenderPass
    {

    public:

        ShadowMapPass(int width, int height);

        void resize_buffers(int width, int height);
        void render(const Renderer& renderer) override;
        void calculate_cascade(const Renderer& renderer, float near, float far, int  i);
        void calculate_cascades(const Renderer& renderer);
        [[nodiscard]] uint32_t get_depth_texture() const;

        [[nodiscard]] uint32_t get_debug_texture(const Renderer& renderer, int cascade_level);

        static const int max_cascades = 8;
        int cascade_idx = 0;
        uint32_t shadow_maps[max_cascades];
        float cascade_ends[max_cascades];
        glm::mat4 cascade_views[max_cascades];
        glm::mat4 cascade_projections[max_cascades];
        glm::vec3 light_positions[max_cascades];

    private:

        void generate_cascade_textures();
        void bind_for_writing(int cascade_level);

    private:

        std::shared_ptr<Shader> m_debug_shader                  = nullptr;
        std::shared_ptr<FrameBufferObject> m_debug_framebuffer  = nullptr;

        std::shared_ptr<Shader> m_shadow_shader     = nullptr;
        std::shared_ptr<FrameBufferObject> m_shadow_framebuffer;
        uint32_t m_depth_texture                    = -1;

        uint32_t m_width;
        uint32_t m_height;

        float m_current_near                        = -1.0f;
        float m_current_far                         = -1.0f;
        int m_current_cascade_level                 = -1;
    };
}
