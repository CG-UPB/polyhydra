#pragma once

#include "../../panels/MeshView.h"
#include "RenderPass.h"

namespace vOS
{
    class MeshView;

    class TransparencyPass_DP : public RenderPass
    {
    public:
        explicit TransparencyPass_DP(MeshView *mesh_view, int width, int height);
        ~TransparencyPass_DP();

        void render(VertexArrayObject* vao, const RenderData& data, int mesh_id) override;
        void render(VertexArrayObject* vao, const RenderData& data, int mesh_id, int pass);
        void resize_buffers(int width, int height);
        //void clear_framebuffer() const;

        GLuint m_texture;

        unsigned int m_width;
        unsigned int m_height;

        FrameBufferObject* m_transparent_framebuffer0 = nullptr;
        FrameBufferObject* m_transparent_framebuffer1 = nullptr;

    private:
        void clean_up_framebuffer();

        void update_draw_texture();

        MeshView* m_mesh_view = nullptr;
        Shader* m_transparency_shader = nullptr;
        VertexArrayObject* m_vao;




    };
}