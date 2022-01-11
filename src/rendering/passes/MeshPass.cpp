
#include <iostream>
#include "glad/glad.h"

#include "MeshPass.h"
#include "../../settings/GlobalViewerSettings.h"

namespace vOS
{
    MeshPass::MeshPass(): m_mesh_shader(Shader::mesh_phong_shader())
    {}

    void MeshPass::render(VertexArrayObject* vao, const RenderData& data)
    {

    }

    void MeshPass::renderMesh(MeshObject* mesh_object, const RenderData& data)
    {
        auto vao = mesh_object->get_vao();

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
            glLineWidth(2);
            glEnable(GL_BLEND);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        m_mesh_shader->bind();

        glm::mat4 positionOffset = glm::translate(-data.mesh.offset);
        glm::mat4 transform = data.camera.world * data.mesh.transform * positionOffset;

        float cell_size = GlobalViewerSettings::getInstance()->m_get_current_cell_size();
        int peel_depth = GlobalViewerSettings::getInstance()->m_get_current_mesh_peel_level();
        int slice_depth = GlobalViewerSettings::getInstance()->m_get_current_mesh_slice_level();
        int slice_coord = 0;
        glm::vec3 min = mesh_object->get_min();
        glm::vec3 max = mesh_object->get_max();
        float slice_min = min.x + slice_depth * 0.1 * (max.x - min.x);
        std::cout << "Min: " << min.x << std::endl;
        std::cout << "Max: " << max.x << std::endl;
        std::cout << "SliceMin: " << slice_min << std::endl;


        // set all of our uniforms
        m_mesh_shader->set_uniform_mat4f("u_Transform", transform);
        m_mesh_shader->set_uniform_mat4f("u_Projection", data.camera.projection);
        m_mesh_shader->set_uniform_mat4f("u_View", data.camera.view);
        m_mesh_shader->set_uniform_vec3f("u_lightPos", data.light.position);
        m_mesh_shader->set_uniform_vec3f("u_camPos", data.camera.position);
        m_mesh_shader->set_uniform_vec3f("u_lightColor", data.light.color);
        m_mesh_shader->set_uniform_vec3f("u_objectColor", data.mesh.color);
        m_mesh_shader->set_uniform_float("u_cell_size", cell_size);
        m_mesh_shader->set_uniform_int("u_peelDepth", peel_depth);
        m_mesh_shader->set_uniform_int("u_sliceCoord", slice_coord);
        m_mesh_shader->set_uniform_float("u_sliceMin", slice_min);


        vao->draw();

//        // render edges on top of the mesh
//        if (!m_render_wireframe)
//        {
//            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//            glEnable(GL_LINE_SMOOTH);
//            glLineWidth(2);
//            glEnable(GL_BLEND);
//
//            // move a small epsilon to the camera, else it would overlay with the mesh
//            float epsilon = 0.02;
//            glm::mat4 temp = glm::translate(glm::vec3(0.0, 0.0, epsilon));
//            transform = temp * data.camera.world * data.mesh.transform * positionOffset;
//            m_mesh_shader->set_uniform_mat4f("u_Transform", transform);
//            m_mesh_shader->set_uniform_vec3f("u_objectColor", glm::vec3(0.0, 0.0, 0.0));
//            vao->draw();
//            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//        }

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