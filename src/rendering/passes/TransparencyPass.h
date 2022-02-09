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

    private:

        void generate_transparency_framebuffer();
        void clean_up_framebuffer();

        MeshView* m_mesh_view = nullptr;
        Shader* m_transparency_shader = nullptr;
        Shader* m_composite_shader = nullptr;

        unsigned int m_width;
        unsigned int m_height;

        unsigned int m_transparent_framebuffer;
        unsigned int m_accumTexture;
        unsigned int m_revealTexture;


    };
}