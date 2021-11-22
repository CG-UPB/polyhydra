
#include "BackgroundPass.h"
#include "../CommonShapes.h"

namespace vOS
{

    BackgroundPass::BackgroundPass(): m_background_color(glm::vec4(0.2, 0.2, 0.2, 1.0))
    {
        m_vao = new VertexArrayObject(CommonShapes::Quad::vertices(2.0f), CommonShapes::Quad::indices());
        m_vao->add_buffer(CommonShapes::Quad::uvs(), 1, 2);
        m_background_shader = Shader::background_shader();
    }

    BackgroundPass::~BackgroundPass()
    {
        delete m_vao;
    }

    void BackgroundPass::render(const VertexArrayObject& vao, const RenderData& data)
    {
        // rendering a solid background color for now, may be changed to something more fancy
        glDisable(GL_BLEND);
        m_background_shader->bind();
        m_background_shader->set_uniform_vec4f("u_color", m_background_color);
        m_vao->draw();
        m_background_shader->unbind();
    }

    void BackgroundPass::set_background_color(const glm::vec4& color)
    {
        m_background_color = color;
    }

    const glm::vec4& BackgroundPass::get_background_color() const
    {
        return m_background_color;
    }
}