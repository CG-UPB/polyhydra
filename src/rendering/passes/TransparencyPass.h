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
        void render_composition();
        void resize_buffers(unsigned int width, unsigned int height);
        void clear_framebuffer() const;

        GLuint m_revealTexture;
        GLuint m_accumTexture;
        unsigned int m_transparent_framebuffer;
        unsigned int m_width;
        unsigned int m_height;
        void generate_transparency_framebuffer(unsigned int width, unsigned int height);

    private:


        void clean_up_framebuffer();

        glm::vec4 m_zeros = glm::vec4(0.0f);
        glm::vec4 m_ones =glm::vec4(1.0f);


        MeshView* m_mesh_view = nullptr;

        Shader* m_transparency_shader = nullptr;
        Shader* m_composite_shader = nullptr;

        VertexArrayObject* m_vao;




    };
}