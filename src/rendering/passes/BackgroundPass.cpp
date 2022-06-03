
#include "BackgroundPass.h"
#include "../meshes/CommonMeshes.h"

namespace vOS
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

    void BackgroundPass::render(std::shared_ptr<VertexArrayObject> vao, const RenderData& data, std::shared_ptr<MeshObject> mesh)
    {
        // Parameters are ignored, as they are not necessary

        // Rendering a simple gradient
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