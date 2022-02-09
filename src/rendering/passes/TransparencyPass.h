#pragma once

#include "../../panels/MeshView.h"
#include "RenderPass.h"

namespace vOS
{
    class MeshView;

    class TransparencyPass : public RenderPass
    {
    public:
        explicit TransparencyPass(MeshView *mesh_view, unsigned int width, unsigned int height);
        ~TransparencyPass();

        void render(VertexArrayObject* vao, const RenderData& data, int mesh_id) override;
        void renderComposition();
        void resize_buffers(unsigned int width, unsigned int height);
        void clear_framebuffer();

        unsigned int m_revealTexture;
        unsigned int m_transparent_framebuffer;
        unsigned int m_accumTexture;

    private:
        void generate_transparency_framebuffer(unsigned int width, unsigned int height);


        void clean_up_framebuffer();
        MeshView* m_mesh_view = nullptr;
        Shader* m_transparency_shader = nullptr;

        Shader* m_composite_shader = nullptr;
        unsigned int m_width;

        VertexArrayObject* m_vao;
        unsigned int m_height;



    };
}