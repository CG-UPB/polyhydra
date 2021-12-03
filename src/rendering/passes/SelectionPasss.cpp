#include "SelectionPasss.h"
#include "glad/glad.h"

#include "MeshPass.h"

namespace vOS {
    SelectionPass::SelectionPass(): m_selection_shader(Shader::selection_shader())
    {}

    void SelectionPass::render(const VertexArrayObject &vao, const RenderData &data)
    {
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);


        glm::mat4 positionOffset = glm::translate(-data.mesh.offset);
        glm::mat4 transform = data.camera.world * data.mesh.transform * positionOffset;

        m_selection_shader->bind();

        m_selection_shader->set_uniform_mat4f("u_mesh_transform", transform);
        m_selection_shader->set_uniform_mat4f("u_projection", data.camera.projection);
        m_selection_shader->set_uniform_mat4f("u_view", data.camera.view);
        m_selection_shader->set_uniform_int("u_selection_offset", data.mesh.selection_offset);

        vao.draw();

        m_selection_shader->unbind();
    }
}