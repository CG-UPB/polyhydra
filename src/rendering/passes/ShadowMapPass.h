
#pragma once

#include "RenderPass.h"
#include "../../panels/MeshView.h"


namespace vOS
{

    class ShadowMapPass : public RenderPass
    {

    public:

        ShadowMapPass(MeshView* mesh_view, int width, int height);
        ~ShadowMapPass();

        void resize_buffers(int width, int height);
        void render(VertexArrayObject* vao, const RenderData& data, int mesh_id) override;
        void calculate_cascade(float near, float far);
        void clear_cascades();

        FrameBufferObject* get_framebuffer() const;
        [[nodiscard]] unsigned int get_shadow_map() const;

        std::vector<glm::mat4> m_cascade_views;
        std::vector<glm::mat4> m_cascade_projections;

    private:

        MeshView* m_mesh_view;
        Shader* m_shadow_shader = nullptr;
        FrameBufferObject* m_shadow_framebuffer = nullptr;
        float m_z_mult = 1.0f;

    };
}
