
#pragma once

#include "RenderPass.h"
#include "../Renderer.h"


namespace vOS
{
    class Renderer;

    class ShadowMapPass : public RenderPass
    {

    public:

        ShadowMapPass(Renderer* renderer, int width, int height);
        ~ShadowMapPass();

        void resize_buffers(int width, int height);
        void render(VertexArrayObject* vao, const RenderData& data, int mesh_id) override;
        void calculate_cascade(float near, float far);
        void clear_cascades();

        [[nodiscard]] FrameBufferObject* get_framebuffer() const;
        [[nodiscard]] unsigned int get_shadow_map() const;

        std::vector<glm::mat4> m_cascade_views;
        std::vector<glm::mat4> m_cascade_projections;

    private:

        Renderer* m_renderer;
        Shader* m_shadow_shader = nullptr;
        FrameBufferObject* m_shadow_framebuffer = nullptr;
        float m_z_mult = 1.2f;

    };
}
