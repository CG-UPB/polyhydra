
#include <iostream>
#include "glad/glad.h"
#include "../../Window.h"
#include "MeshPass.h"
#include "../../util/VecUtil.h"

namespace vOS
{
    MeshPass::MeshPass(Renderer* renderer): m_renderer(renderer)
    {}

    void MeshPass::render(VertexArrayObject* vao, const RenderData& data, int mesh_id)
    {
        // Get MeshObject
        MeshObject* obj = Window::instance().get_mesh_obj(mesh_id);
        if(obj == nullptr)
            return;

        auto settings = GlobalViewerSettings::getInstance();
        bool draw_wireframe = settings->get_mesh_mode() == Wireframe;
        float wireframe_size = settings->get_wireframe_size();
        bool use_vertex_normals = settings->get_mesh_mode() == Phong_Vertexnormals;

        if (draw_wireframe)
        {
            glDisable(GL_CULL_FACE);
        }
        else
        {
            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CCW);
            glCullFace(GL_BACK);
        }

        glDisable(GL_BLEND);
//        glEnable(GL_BLEND);
//        glBlendEquation(GL_FUNC_ADD);
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);

        // Get shader
        auto m_mesh_shader = Shader::get("mesh_phong");

        m_mesh_shader->bind();

        // Transform
        glm::mat4 transform = data.camera.world * obj->get_data().get_transform();
        glm::mat4 l_transform = data.light.world * obj->get_data().get_transform();
        glm::mat4 view_transform = data.camera.view * transform;

        // View Operations
        glm::vec3 view_dir = -glm::normalize(data.camera.get_front());
        auto slice_direction = obj->get_slice_dir(view_transform, view_dir);

        glm::vec3 cam_pos(data.camera.view * glm::vec4(data.camera.position, 1.0));
        //glm::vec3 light_pos(data.camera.view * glm::vec4(data.light.light_dir, 1.0));
        glm::mat3 mvp_ti = glm::mat3(glm::transpose(glm::inverse(view_transform)));
        glm::vec3 light_pos(glm::normalize(mvp_ti * data.light.light_dir));


        // Shader uniforms
        m_mesh_shader->set_uniform_mat4f("u_transform", data.camera.world * obj->get_data().get_transform());
        m_mesh_shader->set_uniform_mat4f("u_projection", data.camera.projection);
        m_mesh_shader->set_uniform_mat4f("u_view", data.camera.view);
        m_mesh_shader->set_uniform_vec3f("u_light_pos", light_pos);
        m_mesh_shader->set_uniform_vec3f("u_cam_pos", cam_pos);
        m_mesh_shader->set_uniform_vec3f("u_light_color", data.light.color);
        m_mesh_shader->set_uniform_float("u_cell_size", obj->get_data().m_cell_size);
        m_mesh_shader->set_uniform_vec4f("u_object_color", obj->get_data().m_color.get_rgba());
        m_mesh_shader->set_uniform_float("u_peel_depth", obj->get_data().m_peel_level);
        m_mesh_shader->set_uniform_float("u_slice_depth", obj->get_data().m_slice_level);
        m_mesh_shader->set_uniform_vec3f("u_min", obj->get_transformed_bb(view_transform).first);
        m_mesh_shader->set_uniform_vec3f("u_max", obj->get_transformed_bb(view_transform).second);
        m_mesh_shader->set_uniform_vec3f("u_slice_direction", obj->get_slice_dir(view_transform, -glm::normalize(data.camera.get_front())));
        m_mesh_shader->set_uniform_bool("u_slice_locked", obj->get_data().m_slice_locked);
        m_mesh_shader->set_uniform_float("u_spec_strength", obj->get_data().m_specular_strength);
        m_mesh_shader->set_uniform_float("u_spec_exponent", obj->get_data().m_specular_exponent);
        m_mesh_shader->set_uniform_float("u_ambient_strength", obj->get_data().m_ambient_strength);
        m_mesh_shader->set_uniform_float("u_diffuse_strength", obj->get_data().m_diffuse_strength);
        m_mesh_shader->set_uniform_bool("u_rounding", obj->get_data().m_rounding_activated);
        m_mesh_shader->set_uniform_float("u_rounding_size", obj->get_data().m_rounding_size);
        m_mesh_shader->set_uniform_vec4f("u_selection_color", obj->get_data().m_selection_color.get_rgba());
        m_mesh_shader->set_uniform_float("u_average_cell_size", obj->get_mvb()->get_average_cell_size());
        m_mesh_shader->set_uniform_int("u_cascade_level", settings->get_cascade_level() - 1);


        m_mesh_shader->set_uniform_int("u_viewport_width", m_renderer->m_viewportPanelWidth);
        m_mesh_shader->set_uniform_int("u_viewport_height", m_renderer->m_viewportPanelHeight);


        float bias_min = 0.0006;
        float bias_max = 0.005;
        float bias_modifier = 0.1;
        m_mesh_shader->set_uniform_float("u_bias_min", bias_min);
        m_mesh_shader->set_uniform_float("u_bias_max", bias_max);
        m_mesh_shader->set_uniform_float("u_bias_modifier", bias_modifier);


        // shadow maps
        auto s = m_renderer->m_shadow_pass;
        for(int i = 0; i < s->max_cascades; i++)
        {
            m_mesh_shader->set_uniform_mat4f("u_light_projection[" + std::to_string(i) +"]", s->cascade_projections[i]);
            m_mesh_shader->set_uniform_mat4f("u_light_view[" + std::to_string(i) +"]", s->cascade_views[i]);
            m_mesh_shader->set_uniform_float("u_cascade_ends[" + std::to_string(i) +"]", s->cascade_ends[i]);
        }

        m_mesh_shader->set_uniform_mat4f("u_light_transform", l_transform);
        m_mesh_shader->set_uniform_mat4f("u_light_projection", data.light.projection);
        m_mesh_shader->set_uniform_mat4f("u_light_view", data.light.view);
        m_mesh_shader->set_uniform_mat4f("u_light_transform", l_transform);
        m_mesh_shader->set_uniform_int("u_viewport_width", m_renderer->m_viewportPanelWidth);
        m_mesh_shader->set_uniform_int("u_viewport_height", m_renderer->m_viewportPanelHeight);

        // settings
        m_mesh_shader->set_uniform_bool("u_draw_wireframe", draw_wireframe);
        m_mesh_shader->set_uniform_bool("u_draw_shadows", settings->get_shadows_activated());
        m_mesh_shader->set_uniform_bool("u_draw_ao", settings->get_ambient_occlusion_activated());
        m_mesh_shader->set_uniform_float("u_wireframe_size", wireframe_size);
        m_mesh_shader->set_uniform_bool("u_use_vertex_normals", use_vertex_normals);

        // input textures
        m_mesh_shader->set_uniform_sampler2D("u_depth_texture", GL_TEXTURE0, m_renderer->m_pre_pass->get_framebuffer()->get_depth_texture());
        m_mesh_shader->set_uniform_sampler2D("u_ssao_texture", GL_TEXTURE1, m_renderer->m_ssao_pass->get_blur_texture());
        m_mesh_shader->set_uniform_sampler2D("u_shadow_texture", GL_TEXTURE2, m_renderer->m_shadow_pass->get_framebuffer()->get_texture(GL_DEPTH_ATTACHMENT));
        m_mesh_shader->set_uniform_sampler2D("u_transparent_shadow_texture", GL_TEXTURE3, m_renderer->m_transparent_shadow_pass->get_framebuffer()->get_texture(GL_DEPTH_ATTACHMENT));
        m_mesh_shader->set_uniform_sampler2D("u_color_filter_texture", GL_TEXTURE4, m_renderer->m_shadow_color_filter_pass->get_framebuffer()->get_texture(GL_COLOR_ATTACHMENT0));
        m_mesh_shader->set_uniform_sampler2D("u_depth_texture", GL_TEXTURE0,m_mesh_view->m_pre_pass->get_framebuffer()->get_depth_texture());
        m_mesh_shader->set_uniform_sampler2D("u_ssao_texture", GL_TEXTURE1,m_mesh_view->m_ssao_pass->get_blur_texture());
        m_mesh_shader->set_uniform_sampler2D("u_transparent_shadow_texture", GL_TEXTURE2,m_mesh_view->m_transparent_shadow_pass->get_framebuffer()->get_texture(GL_DEPTH_ATTACHMENT));
        m_mesh_shader->set_uniform_sampler2D("u_color_filter_texture", GL_TEXTURE3,m_mesh_view->m_shadow_color_filter_pass->get_framebuffer()->get_texture(GL_COLOR_ATTACHMENT0));

        // bind cascaded shadow map
        std::vector<unsigned int> bindings = {GL_TEXTURE4, GL_TEXTURE5, GL_TEXTURE6, GL_TEXTURE7,
                                              GL_TEXTURE8, GL_TEXTURE9, GL_TEXTURE10, GL_TEXTURE11};
        for(int i = 0; i < s->max_cascades; i++)
        {
            m_mesh_shader->set_uniform_sampler2D("u_shadow_texture[" + std::to_string(i) + "]", bindings[i],s->shadow_maps[i]);
        }

        // wireframe mode should always be non-rounded
        if (draw_wireframe)
        {
            obj->get_mvb()->get_vao_by_face()->draw();
        }
        else
        {
            vao->draw();
        }

        m_mesh_shader->unbind();
    }


}