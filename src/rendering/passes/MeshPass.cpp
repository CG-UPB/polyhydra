
#include <iostream>
#include "glad/glad.h"
#include "../../Window.h"
#include "MeshPass.h"
#include "../../settings/GlobalViewerSettings.h"

namespace vOS
{


    void MeshPass::render(VertexArrayObject* vao, const RenderData& data, int mesh_id)
    {
        // Get Mesh
        MeshObject* obj = Window::instance().get_mesh_obj(mesh_id);
        if(obj == nullptr)
            return;

        // Activate Wireframe mode if desired
        std::string rendering_mode = obj->get_data().rendering_mode;
        bool render_in_wireframe_mode = false;
        if(rendering_mode == "mesh_wireframe") {
            rendering_mode = "mesh_phong";
            render_in_wireframe_mode = true;
        }

        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);

        if (render_in_wireframe_mode)
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

        auto m_mesh_shader = Shader::get(rendering_mode);

        m_mesh_shader->bind();

        glm::mat4 positionOffset = glm::translate(-obj->get_data().offset);
        glm::mat4 transform = data.camera.world * obj->get_data().transform * positionOffset;

        float cell_size = Window::instance().get_mesh_cell_size(mesh_id);
        int peel_depth = Window::instance().get_mesh_peel_level(mesh_id);
        float slice_depth = Window::instance().get_mesh_slice_level(mesh_id);
        auto bb = obj->get_transformed_bb(transform);
        auto min = bb.first;
        auto max = bb.second;

        glm::mat4 view_inv = glm::inverse(data.camera.view);
        glm::vec3 view_dir = {view_inv[2][0], view_inv[2][1], view_inv[2][2]};
        auto slice_direction = obj->get_slice_dir(transform, view_dir);

        // set all of our uniforms
        m_mesh_shader->set_uniform_mat4f("u_Transform", transform);
        m_mesh_shader->set_uniform_mat4f("u_Projection", data.camera.projection);
        m_mesh_shader->set_uniform_mat4f("u_View", data.camera.view);
        m_mesh_shader->set_uniform_vec3f("u_lightPos", data.light.position);
        m_mesh_shader->set_uniform_vec3f("u_camPos", data.camera.position);
        m_mesh_shader->set_uniform_vec3f("u_lightColor", data.light.color);
        m_mesh_shader->set_uniform_float("u_cell_size", cell_size);
        m_mesh_shader->set_uniform_vec3f("u_objectColor", obj->get_data().m_color.get());
        m_mesh_shader->set_uniform_int("u_peel_depth", peel_depth);
        m_mesh_shader->set_uniform_float("u_slice_depth", slice_depth);
        m_mesh_shader->set_uniform_vec3f("u_min", min);
        m_mesh_shader->set_uniform_vec3f("u_max", max);
        m_mesh_shader->set_uniform_vec3f("u_slice_direction", slice_direction);
        m_mesh_shader->set_uniform_bool("u_slice_locked", obj->get_data().m_slice_locked);

        vao->draw();

        m_mesh_shader->unbind();

        if (render_in_wireframe_mode)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

}