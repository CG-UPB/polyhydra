#pragma once

#include "../../panels/MeshView.h"
#include "RenderPass.h"

namespace vOS
{
    class MeshView;

    class TransparencyPass_DP : public RenderPass
    {
    public:
        explicit TransparencyPass_DP(MeshView *mesh_view, unsigned int width, unsigned int height);
        ~TransparencyPass_DP();

        void render(VertexArrayObject* vao, const RenderData& data, int mesh_id) override;
        void render_composition();
        void resize_buffers(unsigned int width, unsigned int height);
        //void clear_framebuffer() const;

        GLuint m_min_depth_texture;
        GLuint m_front_texture;
        GLuint m_back_texture;
        GLuint m_depth_texture;

        unsigned int m_transparent_framebuffer;
        unsigned int m_width;
        unsigned int m_height;

        void generate_transparency_framebuffer(unsigned int width, unsigned int height);

    private:


        void clean_up_framebuffer();

        MeshView* m_mesh_view = nullptr;

        Shader* m_transparency_shader = nullptr;
        Shader* m_composite_shader = nullptr;

        VertexArrayObject* m_vao;




    };
}