
#include <iostream>
#include "glad/glad.h"
#include "../../Window.h"
#include "MeshPass.h"

namespace vOS
{
    MeshPass::MeshPass(MeshView* mesh_view): m_mesh_view(mesh_view)
    {}

    void MeshPass::render(VertexArrayObject* vao, const RenderData& data, int mesh_id)
    {
        // Get MeshObject
        MeshObject* obj = Window::instance().get_mesh_obj(mesh_id);
        if(obj == nullptr)
            return;

        // Activate Wireframe mode if desired
        std::string rendering_mode = obj->get_data().m_rendering_mode;
        bool render_in_wireframe_mode = false;
        if(rendering_mode == "mesh_wireframe") {
            rendering_mode = "mesh_phong";
            render_in_wireframe_mode = true;
        }

        // Gl Setup
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);

        // Additonal Setup necessary if in wireframe mode
        if (render_in_wireframe_mode)
        {
            glDisable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else
        {
            glEnable(GL_BLEND);
        }


        // Get shader
        auto m_mesh_shader = Shader::get(rendering_mode);

        m_mesh_shader->bind();

        // Transform
        glm::mat4 positionOffset = glm::translate(-obj->get_data().m_offset);
        glm::mat4 transform = data.camera.world * obj->get_data().get_transform() * positionOffset;
        glm::mat4 l_transform = data.light.world * obj->get_data().get_transform() * positionOffset;

        // Cell operations
        float cell_size = obj->get_data().m_cell_size;
        int peel_depth = obj->get_data().m_peel_level;
        float slice_depth = obj->get_data().m_slice_level;

        auto bb = obj->get_transformed_bb(transform);
        auto min = bb.first;
        auto max = bb.second;

        // View Operations
        glm::mat4 view_inv = glm::inverse(data.camera.view);
        glm::vec3 view_dir = {view_inv[2][0], view_inv[2][1], view_inv[2][2]};
        auto slice_direction = obj->get_slice_dir(transform, view_dir);
        //auto slice_direction = data.camera.m_camera_front;

        // Shader uniforms
        m_mesh_shader->set_uniform_mat4f("u_Transform", transform);
        m_mesh_shader->set_uniform_mat4f("u_Projection", data.camera.projection);
        m_mesh_shader->set_uniform_mat4f("u_View", data.camera.view);
        m_mesh_shader->set_uniform_vec3f("u_lightPos", data.light.position);
        m_mesh_shader->set_uniform_vec3f("u_camPos", data.camera.position);
        m_mesh_shader->set_uniform_vec3f("u_lightColor", data.light.color);
        m_mesh_shader->set_uniform_float("u_cell_size", cell_size);
        m_mesh_shader->set_uniform_vec4f("u_objectColor", obj->get_data().m_color.get_rgba());
        m_mesh_shader->set_uniform_int("u_peel_depth", peel_depth);
        m_mesh_shader->set_uniform_float("u_slice_depth", slice_depth);
        m_mesh_shader->set_uniform_vec3f("u_min", min);
        m_mesh_shader->set_uniform_vec3f("u_max", max);
        m_mesh_shader->set_uniform_vec3f("u_slice_direction", slice_direction);
        m_mesh_shader->set_uniform_bool("u_slice_locked", obj->get_data().m_slice_locked);
        m_mesh_shader->set_uniform_float("u_spec_strength", obj->get_data().m_specular_strength);
        m_mesh_shader->set_uniform_float("u_spec_exponent", obj->get_data().m_specular_exponent);
        m_mesh_shader->set_uniform_float("u_ambient_strength", obj->get_data().m_ambient_strength);
        m_mesh_shader->set_uniform_float("u_diffuse_strength", obj->get_data().m_diffuse_strength);
        m_mesh_shader->set_uniform_bool("u_draw_wireframe", render_in_wireframe_mode);
        m_mesh_shader->set_uniform_bool("u_rounding", data.rounding.active);
        m_mesh_shader->set_uniform_float("u_rounding_size", data.rounding.size);
        m_mesh_shader->set_uniform_vec4f("u_selection_color", obj->get_data().m_selection_color.get_rgba());

        m_mesh_shader->set_uniform_mat4f("u_light_projection", data.light.projection);
        m_mesh_shader->set_uniform_mat4f("u_light_view", data.light.view);
        m_mesh_shader->set_uniform_mat4f("u_light_transform", l_transform);

//        m_mesh_shader->set_uniform_mat4f("u_light_projection", data.camera.projection);
//        m_mesh_shader->set_uniform_mat4f("u_light_view", data.camera.view);

        m_mesh_shader->set_uniform_int("u_viewport_width", m_mesh_view->m_viewportPanelWidth);
        m_mesh_shader->set_uniform_int("u_viewport_height", m_mesh_view->m_viewportPanelHeight);

        m_mesh_shader->set_uniform_sampler2D("u_depth_texture", GL_TEXTURE0,
                                             m_mesh_view->m_pre_pass->get_framebuffer()->get_depth_texture());
        m_mesh_shader->set_uniform_sampler2D("u_ssao_texture", GL_TEXTURE1,m_mesh_view->m_ssao_pass->get_blur_texture());
        //m_mesh_shader->set_uniform_sampler2D("u_position", GL_TEXTURE2,m_mesh_view->m_pre_pass->get_framebuffer()->get_position_texture());

        m_mesh_shader->set_uniform_sampler2D("u_shadow_texture", GL_TEXTURE2,m_mesh_view->m_shadow_pass->get_framebuffer()->get_texture(GL_DEPTH_ATTACHMENT));
        m_mesh_shader->set_uniform_sampler2D("u_transparent_shadow_texture", GL_TEXTURE3,m_mesh_view->m_transparent_shadow_pass->get_framebuffer()->get_texture(GL_DEPTH_ATTACHMENT));
        m_mesh_shader->set_uniform_sampler2D("u_color_filter_texture", GL_TEXTURE4,m_mesh_view->m_shadow_color_filter_pass->get_framebuffer()->get_texture(GL_COLOR_ATTACHMENT0));

        if (data.rounding.active)
        {
            obj->get_mvb()->get_vao_rounded()->draw();
        }
        else
        {
            vao->draw();
        }

        m_mesh_shader->unbind();

        if (render_in_wireframe_mode)
        {
            glEnable(GL_CULL_FACE);
        }
    }

}