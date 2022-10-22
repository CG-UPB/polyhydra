#include "MeshPass.h"
#include "mesh/MeshProperties.h"
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

            bool is_bezier_mesh = mesh->is_bezier_mesh();
            // Currently, cells sometimes appear hollow if CULL_FACE is not
            // disabled for Bézier meshes
            if (draw_wireframe || is_bezier_mesh || !mesh->get_data().use_back_face_culling)
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
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);

            // Get shader
            auto m_mesh_shader = Shader::get("mesh_phong");

            m_mesh_shader->bind();

            auto cam = renderer.camera;
            auto light = AppState::settings.light;

            // Transform
            glm::mat4 transform = cam->world * mesh->get_data().get_transform();
            glm::mat4 l_transform = mesh->get_data().get_transform();
            glm::mat4 view_transform = cam->view * transform;

            // volumeshOS Operations
            glm::vec3 view_dir = -glm::normalize(cam->get_front());
            auto slice_direction = mesh->get_slice_dir(transform, view_dir);

            glm::vec3 cam_pos(cam->position);
            glm::vec3 light_pos(glm::normalize(light.direction));


            // Shader uniforms
            m_mesh_shader->set_uniform_mat4f("u_transform", transform);
            m_mesh_shader->set_uniform_mat4f("u_projection", cam->projection);
            m_mesh_shader->set_uniform_mat4f("u_view", cam->view);
            m_mesh_shader->set_uniform_vec3f("u_view_dir", glm::normalize(cam->target - cam->position));
            m_mesh_shader->set_uniform_vec3f("u_light_pos", light_pos);
            m_mesh_shader->set_uniform_vec3f("u_cam_pos", cam_pos);
            m_mesh_shader->set_uniform_vec3f("u_light_color", light.color);
            m_mesh_shader->set_uniform_float("u_cell_size", mesh->get_data().cell_size);
            m_mesh_shader->set_uniform_vec4f("u_object_color", mesh->get_data().color);
            m_mesh_shader->set_uniform_float("u_peel_depth", mesh->get_data().peel_level);
            m_mesh_shader->set_uniform_float("u_max_peel_depth", mesh->get_data().max_peel_depth);
            m_mesh_shader->set_uniform_bool("u_reverse_peeling", mesh->get_data().reverse_peeling);
            m_mesh_shader->set_uniform_float("u_slice_depth", mesh->get_data().slice_level);
            m_mesh_shader->set_uniform_vec3f("u_min", mesh->get_world_bb(view_transform).first);
            m_mesh_shader->set_uniform_vec3f("u_max", mesh->get_world_bb(view_transform).second);
            m_mesh_shader->set_uniform_vec3f("u_slice_direction", slice_direction);
            m_mesh_shader->set_uniform_bool("u_slice_locked", mesh->get_data().slice_locked);

            m_mesh_shader->set_uniform_bool("u_use_base_color", mesh->get_data().use_base_color);
            m_mesh_shader->set_uniform_bool("u_two_sided_lighting", mesh->get_data().use_two_sided_lighting);
            m_mesh_shader->set_uniform_float("u_spec_strength", mesh->get_data().specular_strength);
            m_mesh_shader->set_uniform_float("u_spec_exponent", mesh->get_data().specular_exponent);
            m_mesh_shader->set_uniform_float("u_ambient_strength", mesh->get_data().ambient_strength);
            m_mesh_shader->set_uniform_float("u_diffuse_strength", mesh->get_data().diffuse_strength);

            m_mesh_shader->set_uniform_bool("u_use_pbr", mesh->get_data().use_pbr);
            m_mesh_shader->set_uniform_float("u_metallic", mesh->get_data().metallic);
            m_mesh_shader->set_uniform_float("u_roughness", mesh->get_data().roughness);
            m_mesh_shader->set_uniform_float("u_light_intensity", light.intensity);
            m_mesh_shader->set_uniform_float("u_gamma", settings.post_processing.gamma);
            m_mesh_shader->set_uniform_vec3f("u_ground_color", settings.ground.solid_color);
            m_mesh_shader->set_uniform_vec3f("u_background_color", settings.sky.sky_color);

            m_mesh_shader->set_uniform_float("u_shadow_strength", settings.shadow.shadow_strength);
            m_mesh_shader->set_uniform_float("u_softness", settings.shadow.softness);


            // Do not use rounding on Bézier meshes.
            m_mesh_shader->set_uniform_bool("u_rounding", (is_bezier_mesh) ? false : mesh->get_data().rounding_active);
            m_mesh_shader->set_uniform_float("u_rounding_size", mesh->get_data().rounding_size);
            m_mesh_shader->set_uniform_vec4f("u_selection_color", mesh->get_data().selection_color);
            m_mesh_shader->set_uniform_float("u_average_cell_size", mesh->get_mvb()->get_average_cell_size());
            m_mesh_shader->set_uniform_int("u_cascade_level", settings.num_shadow_cascades - 1);


            m_mesh_shader->set_uniform_int("u_viewport_width", renderer.frame.width);
            m_mesh_shader->set_uniform_int("u_viewport_height", renderer.frame.height);

            m_mesh_shader->set_uniform_float("u_near", cam->near);
            m_mesh_shader->set_uniform_float("u_far", cam->far);


//            m_bias_min = 0.00000001f;
//            m_bias_max = 0.000003f;

//            m_bias_min = 0.000001f;
//            m_bias_max = 0.0001f;

            m_bias_min = 0.0000001f;
            m_bias_max = 0.003f;

            m_mesh_shader->set_uniform_float("u_bias_min", m_bias_min);
            m_mesh_shader->set_uniform_float("u_bias_max", m_bias_max);
            m_mesh_shader->set_uniform_float("u_bias_modifier", m_bias_modifier);


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
            m_mesh_shader->set_uniform_float("u_light_size", settings.shadow.penumbra_scale);



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

            m_mesh_shader->set_uniform_sampler2DArray("u_shadow_texture", GL_TEXTURE4, s->get_depth_texture());

            m_mesh_shader->set_uniform_bool("u_is_bezier_mesh", is_bezier_mesh);
            if(is_bezier_mesh)
            {
                mesh->get_mtb()->bind();
                // Use Bezier Mesh Property to set uniform.
                m_mesh_shader->set_uniform_int("u_bezier_degree", *mesh->get_ovm()->request_mesh_property<int>(MeshProperties::PROP_BEZIER_DEGREE).begin());

                // GL_TEXTURE12 is used for control points storage.
                m_mesh_shader->set_uniform_int("u_control_points_tb", 12);
                // Use tessellation level value from toolbar.
                m_mesh_shader->set_uniform_int("u_bezier_tessellation_level", mesh->get_data().tessellation_level);
            }

            // wireframe mode should always be non-rounded
            if (draw_wireframe)
            {
                mesh->get_mvb()->get_vao_by_face()->draw_patches();
            }
            else
            {
                auto vao = mesh->get_vao();
                if (mesh->get_data().rounding_active && !is_bezier_mesh)
                {
                    vao = mesh->get_mvb()->get_vao_rounded();
                }
                vao->draw_patches();
            }

            m_mesh_shader->unbind();
        }
        renderer.buffers.target_framebuffer_ms->unbind();
    }
}