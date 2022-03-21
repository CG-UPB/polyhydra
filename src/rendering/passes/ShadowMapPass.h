
#pragma once

#include "RenderPass.h"
#include <vector>
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
        void bind_for_writing(int cascade_idx);
        void render(VertexArrayObject* vao, const RenderData& data, int mesh_id) override;
        void calculate_cascade(float near, float far, int  i);
        void calculate_cascades(float near, float far, int cascade_levels);
        void set_cascade_index(int idx){cascade_idx = idx;};
        void clear_cascades();

        [[nodiscard]] FrameBufferObject* get_framebuffer() const;
        [[nodiscard]] unsigned int get_shadow_map() const;

        static const int max_cascades = 8;
        int cascade_idx = 0;
        unsigned int shadow_maps[max_cascades];
        float cascade_ends[max_cascades];
        glm::mat4 cascade_views[max_cascades];
        glm::mat4 cascade_projections[max_cascades];
        glm::vec3 light_positions[max_cascades];

    private:

        Renderer* m_renderer;
        Shader* m_shadow_shader = nullptr;
        FrameBufferObject* m_shadow_framebuffer = nullptr;
        float m_z_mult = 2.5f;

    };
}
