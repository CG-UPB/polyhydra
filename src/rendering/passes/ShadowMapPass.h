
#pragma once

#include "RenderPass.h"
#include "../../panels/MeshView.h"
#include <vector>


namespace vOS
{

    class ShadowMapPass : public RenderPass
    {

    public:

        ShadowMapPass(MeshView* mesh_view, int width, int height);
        ~ShadowMapPass();

        void resize_buffers(int width, int height);
        void bind_for_writing(int cascade_idx);
        void render(VertexArrayObject* vao, const RenderData& data, int mesh_id) override;
        void calculate_cascade(float near, float far, int  i);
        void calculate_cascades(float near, float far, int cascade_levels);
        void clear_cascades();

        [[nodiscard]] FrameBufferObject* get_framebuffer() const;
        [[nodiscard]] unsigned int get_shadow_map() const;

        static const int max_cascades = 12;
        std::vector<unsigned int> shadow_maps;
        std::vector<float> cascade_ends;
        std::vector<glm::mat4> cascade_views;
        std::vector<glm::mat4> cascade_projections;

    private:
        MeshView* m_mesh_view;
        Shader* m_shadow_shader = nullptr;
        FrameBufferObject* m_shadow_framebuffer = nullptr;
        float m_z_mult = 1.2f;

    };
}
