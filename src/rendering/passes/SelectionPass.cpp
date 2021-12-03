#include "SelectionPass.h"
#include "glad/glad.h"

#include "MeshPass.h"
#include "../shapes/CommonMeshes.h"

namespace vOS {
    SelectionPass::SelectionPass(): m_selection_shader(Shader::selection_shader())
    {
        m_selection_shader = Shader::selection_shader();
        m_selection_shape_shader = Shader::selection_shape_shader();
    }

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

        // draw faces
        m_selection_shader->bind();

        m_selection_shader->set_uniform_mat4f("u_mesh_transform", transform);
        m_selection_shader->set_uniform_mat4f("u_projection", data.camera.projection);
        m_selection_shader->set_uniform_mat4f("u_view", data.camera.view);

        vao.draw();

        // draw spheres for each vertex
        m_selection_shape_shader->bind();

        m_selection_shape_shader->set_uniform_mat4f("u_mesh_transform", transform);
        m_selection_shape_shader->set_uniform_mat4f("u_projection", data.camera.projection);
        m_selection_shape_shader->set_uniform_mat4f("u_view", data.camera.view);

        m_sphere_vao->draw_instanced(m_num_vertices);

        m_selection_shape_shader->unbind();
    }

    void SelectionPass::render_mesh(MeshObject* mesh, const RenderData& data)
    {
        if (mesh != nullptr && mesh->get_vao() != nullptr)
        {
            m_sphere_vao = mesh->get_sphere_vao();
            m_num_vertices = mesh->get_num_vertices();
            render(*mesh->get_vao(), data);
        }
    }
}