
#include "glad/glad.h"

#include "BackgroundPass.h"
#include "../meshes/CommonMeshes.h"

namespace vOS
{

    BackgroundPass::BackgroundPass():
        m_top_color(glm::vec4(0.1, 0.1, 0.1, 1.0)),
        m_bottom_color(glm::vec4(0.2, 0.2, 0.2, 1.0))
    {
        m_vao = new VertexArrayObject(CommonMeshes::PlaneXY::vertices(2.0f, 2.0f), CommonMeshes::PlaneXY::indices());
        m_vao->add_attribute(CommonMeshes::PlaneXY::uvs(), 1, 2);
        m_background_shader = Shader::background_shader();
    }

    BackgroundPass::~BackgroundPass()
    {
        delete m_vao;
    }

    void BackgroundPass::render(VertexArrayObject* vao, const RenderData& data, int mesh_id)
    {
        // rendering a simple gradient for now, may be changed to something more fancy
        glDisable(GL_BLEND);
        glDepthMask(GL_FALSE);
        m_background_shader->bind();
        m_background_shader->set_uniform_vec4f("u_top_color", m_top_color);
        m_background_shader->set_uniform_vec4f("u_bottom_color", m_bottom_color);
        m_vao->draw();
        m_background_shader->unbind();
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