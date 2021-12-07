
#include "glad/glad.h"

#include "MeshPass.h"

namespace vOS
{
    MeshPass::MeshPass(): m_mesh_shader(Shader::mesh_phong_shader())
    {}

    void MeshPass::render(const VertexArrayObject& vao, const RenderData& data)
    {
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);

        if (m_render_wireframe)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glEnable(GL_LINE_SMOOTH);
            glLineWidth(1);
            glEnable(GL_BLEND);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        m_mesh_shader->bind();

        glm::mat4 positionOffset = glm::translate(-data.mesh.offset);
        glm::mat4 transform = data.camera.world * data.mesh.transform * positionOffset;

        // set all of our uniforms
        m_mesh_shader->set_uniform_mat4f("u_Transform", transform);
        m_mesh_shader->set_uniform_mat4f("u_Projection", data.camera.projection);
        m_mesh_shader->set_uniform_mat4f("u_View", data.camera.view);
        m_mesh_shader->set_uniform_vec3f("u_lightPos", data.light.position);
        m_mesh_shader->set_uniform_vec3f("u_camPos", data.camera.position);
        m_mesh_shader->set_uniform_vec3f("u_lightColor", data.light.color);
        m_mesh_shader->set_uniform_vec3f("u_objectColor", data.mesh.color);

        vao.draw();

        m_mesh_shader->unbind();

        if (m_render_wireframe)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    void MeshPass::set_wireframe_mode(bool mode)
    {
        m_render_wireframe = mode;
    }

    bool MeshPass::get_wireframe_mode() const
    {
        return m_render_wireframe;
    }

    void MeshPass::set_use_phong(bool use)
    {
        m_use_phong = use;
    }

    bool MeshPass::get_use_phong() const
    {
        return m_use_phong;
    }
}