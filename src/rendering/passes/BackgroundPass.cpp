
#include "BackgroundPass.h"
#include "../meshes/CommonMeshes.h"
#include "rendering/gl/VertexArrayObject.h"
#include "rendering/gl/Shader.h"
#include "../Renderer.h"

namespace volumeshOS::Internal
{

    BackgroundPass::BackgroundPass():
        m_top_color(glm::vec4(1.0,1.0,1.0,1.0)),
        m_bottom_color(glm::vec4(0.8, 0.8, 0.8, 1.0))
    {
        // Create plane mesh
        m_vao = std::make_unique<VertexArrayObject>(CommonMeshes::PlaneXY::vertices(2.0f, 2.0f), CommonMeshes::PlaneXY::indices());
        m_vao->add_attribute(CommonMeshes::PlaneXY::uvs(), 1, 2);
        m_background_shader = Shader::background_shader();
    }


    void BackgroundPass::render(const Renderer& renderer)
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDisable(GL_BLEND);
        glDepthMask(GL_FALSE);

        renderer.buffers.target_framebuffer_ms->bind();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Rendering a simple gradient
        m_background_shader->bind();
        m_background_shader->set_uniform_vec4f("u_top_color", m_top_color);
        m_background_shader->set_uniform_vec4f("u_bottom_color", m_bottom_color);
        m_vao->draw();
        m_background_shader->unbind();

        renderer.buffers.target_framebuffer_ms->unbind();
    }

    void BackgroundPass::set_background_color(const glm::vec4& color)
    {
        m_top_color = color;
    }

    const glm::vec4& BackgroundPass::get_background_color() const
    {
        return m_top_color;
    }
}