#include "MeshPass.h"
#include "rendering/Renderer.h"

namespace volumeshOS::Internal
{

    void MeshPass::render(const Renderer& renderer)
    {
        renderer.buffers.target_framebuffer_ms->bind();
        for (const auto& mesh : renderer.render_list)
        {
            auto& settings = AppState::settings;
            bool draw_wireframe = settings.rendering_mode == RenderingMode::WIREFRAME;
            float wireframe_size = settings.wireframe_size;
            bool use_vertex_normals = settings.rendering_mode == RenderingMode::PHONG_VERTEX_NORMALS;

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

            auto cam = renderer.camera;
            auto light = renderer.light;

            // Transform
            glm::mat4 transform = cam->world * mesh->get_data().get_transform();
            glm::mat4 l_transform = light.world * mesh->get_data().get_transform();
            glm::mat4 view_transform = cam->view * transform;

            // volumeshOS Operations
            glm::vec3 view_dir = -glm::normalize(cam->get_front());
            auto slice_direction = mesh->get_slice_dir(transform, view_dir);

            glm::vec3 cam_pos(cam->view * glm::vec4(cam->position, 1.0));
            //glm::vec3 light_pos(data.camera.view * glm::vec4(data.light.light_dir, 1.0));
            glm::mat3 mvp_ti = glm::mat3(glm::transpose(glm::inverse(cam->view)));
            glm::vec3 light_pos(glm::normalize(mvp_ti * light.light_dir));


            // Shader uniforms
            m_mesh_shader->set_uniform_mat4f("u_transform", transform);
            m_mesh_shader->set_uniform_mat4f("u_projection", cam->projection);
            m_mesh_shader->set_uniform_mat4f("u_view", cam->view);
            m_mesh_shader->set_uniform_vec3f("u_light_pos", light_pos);
            m_mesh_shader->set_uniform_vec3f("u_cam_pos", cam_pos);
            m_mesh_shader->set_uniform_vec3f("u_light_color", light.color);
            m_mesh_shader->set_uniform_float("u_cell_size", mesh->get_data().cell_size);
            m_mesh_shader->set_uniform_vec4f("u_object_color", mesh->get_data().color);
            m_mesh_shader->set_uniform_float("u_peel_depth", mesh->get_data().peel_level);
            m_mesh_shader->set_uniform_float("u_slice_depth", mesh->get_data().slice_level);
            m_mesh_shader->set_uniform_vec3f("u_min", mesh->get_world_bb(view_transform).first);
            m_mesh_shader->set_uniform_vec3f("u_max", mesh->get_world_bb(view_transform).second);
            m_mesh_shader->set_uniform_vec3f("u_slice_direction", slice_direction);
            m_mesh_shader->set_uniform_bool("u_slice_locked", mesh->get_data().slice_locked);
            m_mesh_shader->set_uniform_float("u_spec_strength", mesh->get_data().specular_strength);
            m_mesh_shader->set_uniform_float("u_spec_exponent", mesh->get_data().specular_exponent);
            m_mesh_shader->set_uniform_float("u_ambient_strength", mesh->get_data().ambient_strength);
            m_mesh_shader->set_uniform_float("u_diffuse_strength", mesh->get_data().diffuse_strength);
            m_mesh_shader->set_uniform_bool("u_rounding", mesh->get_data().rounding_active);
            m_mesh_shader->set_uniform_float("u_rounding_size", mesh->get_data().rounding_size);
            m_mesh_shader->set_uniform_vec4f("u_selection_color", mesh->get_data().selection_color);
            m_mesh_shader->set_uniform_float("u_average_cell_size", mesh->get_mvb()->get_average_cell_size());
            m_mesh_shader->set_uniform_int("u_cascade_level", settings.num_shadow_cascades - 1);


            m_mesh_shader->set_uniform_int("u_viewport_width", renderer.frame.width);
            m_mesh_shader->set_uniform_int("u_viewport_height", renderer.frame.height);


            float bias_min = 0.00005;
            float bias_max = 0.004;
            float bias_modifier = 0.1;

//        if(ImGui::Begin("Shadow"))
//        {
//            ImGui::SliderFloat("bias_min", &bias_min, 0.000001, 0.005f);
//            ImGui::SliderFloat("bias_max", &bias_max, 0.00005f, 0.005f);
//            ImGui::End();
//        }

            m_mesh_shader->set_uniform_float("u_bias_min", bias_min);
            m_mesh_shader->set_uniform_float("u_bias_max", bias_max);
            m_mesh_shader->set_uniform_float("u_bias_modifier", bias_modifier);


            // shadow maps
            auto s = renderer.passes.shadow_pass;
            for (int i = 0; i < s->max_cascades; i++)
            {
                m_mesh_shader->set_uniform_mat4f("u_light_projection[" + std::to_string(i) + "]",
                                                 s->cascade_projections[i]);
                m_mesh_shader->set_uniform_mat4f("u_light_view[" + std::to_string(i) + "]", s->cascade_views[i]);
                m_mesh_shader->set_uniform_float("u_cascade_ends[" + std::to_string(i) + "]", s->cascade_ends[i]);
            }

            m_mesh_shader->set_uniform_mat4f("u_light_transform", l_transform);


            // settings
            m_mesh_shader->set_uniform_bool("u_draw_wireframe", draw_wireframe);
            m_mesh_shader->set_uniform_bool("u_draw_shadows", settings.shadows_active);
            m_mesh_shader->set_uniform_bool("u_draw_ao", settings.ssao_active);
            m_mesh_shader->set_uniform_float("u_wireframe_size", wireframe_size);
            m_mesh_shader->set_uniform_bool("u_use_vertex_normals", use_vertex_normals);

            // input textures
            m_mesh_shader->set_uniform_sampler2D("u_depth_texture", GL_TEXTURE0,
                                                 renderer.passes.pre_pass->get_framebuffer()->get_depth_texture());
            m_mesh_shader->set_uniform_sampler2D("u_ssao_texture", GL_TEXTURE1,
                                                 renderer.passes.ssao_pass->get_blur_texture());

            // bind cascaded shadow map
            std::vector<unsigned int> bindings = {GL_TEXTURE4, GL_TEXTURE5, GL_TEXTURE6, GL_TEXTURE7,
                                                  GL_TEXTURE8, GL_TEXTURE9, GL_TEXTURE10, GL_TEXTURE11};
            for (int i = 0; i < s->max_cascades; i++)
            {
                m_mesh_shader->set_uniform_sampler2D("u_shadow_texture[" + std::to_string(i) + "]", bindings[i],
                                                     s->shadow_maps[i]);
            }

            // wireframe mode should always be non-rounded
            if (draw_wireframe)
            {
                mesh->get_mvb()->get_vao_by_face()->draw();
            }
            else
            {
                auto vao = mesh->get_vao();
                if (mesh->get_data().rounding_active)
                {
                    vao = mesh->get_mvb()->get_vao_rounded();
                }
                vao->draw();
            }

            m_mesh_shader->unbind();
        }
        renderer.buffers.target_framebuffer_ms->unbind();
    }
}