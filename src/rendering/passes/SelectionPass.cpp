#include "SelectionPass.h"
#include "glad/glad.h"

#include "MeshPass.h"
#include "../meshes/CommonMeshes.h"

namespace vOS {
    SelectionPass::SelectionPass(): m_selection_shader(Shader::selection_shader())
    {
        m_selection_shader = Shader::selection_shader();
        m_selection_sphere_shader = Shader::selection_sphere_shader();
        m_selection_cylinder_shader = Shader::selection_cylinder_shader();
    }

    void SelectionPass::render(VertexArrayObject* vao, const RenderData &data)
    {
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        glm::mat4 positionOffset = glm::translate(-data.mesh.offset);
        glm::mat4 transform = data.camera.world * data.mesh.transform * positionOffset;

        // draw faces
        m_selection_shader->bind();

        m_selection_shader->set_uniform_mat4f("u_mesh_transform", transform);
        m_selection_shader->set_uniform_mat4f("u_projection", data.camera.projection);
        m_selection_shader->set_uniform_mat4f("u_view", data.camera.view);

        vao->draw();

        glDisable(GL_CULL_FACE);
        glDepthMask(GL_FALSE);

        // draw cylinders for each edge
        m_selection_cylinder_shader->bind();

        m_selection_cylinder_shader->set_uniform_mat4f("u_mesh_transform", transform);
        m_selection_cylinder_shader->set_uniform_mat4f("u_projection", data.camera.projection);
        m_selection_cylinder_shader->set_uniform_mat4f("u_view", data.camera.view);

        m_cylinder_vao->draw_instanced(m_num_edges);

        glDepthMask(GL_TRUE);

        // draw spheres for each vertex
        m_selection_sphere_shader->bind();

        //std::cout << "scale: " << glm::length(transform[0]) << std::endl;

        m_selection_sphere_shader->set_uniform_mat4f("u_mesh_transform", transform);
        m_selection_sphere_shader->set_uniform_mat4f("u_projection", data.camera.projection);
        m_selection_sphere_shader->set_uniform_mat4f("u_view", data.camera.view);
        m_selection_sphere_shader->set_uniform_vec3f("u_cam_pos", data.camera.position);

        m_sphere_vao->draw_instanced(m_num_vertices);

        m_selection_sphere_shader->unbind();
    }

    void SelectionPass::render_mesh(MeshObject* mesh, const RenderData& data)
    {
        if (mesh != nullptr && mesh->get_vao() != nullptr)
        {
            m_sphere_vao = mesh->get_sphere_vao();
            m_num_vertices = mesh->get_num_visible_vertices();
            m_cylinder_vao = mesh->get_cylinder_vao();
            m_num_edges = mesh->get_num_visible_edges();
            render(mesh->get_vao(), data);
        }
    }
}