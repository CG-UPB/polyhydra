
#include "GroundPass.h"
#include "../meshes/CommonMeshes.h"
#include "rendering/gl/VertexArrayObject.h"
#include "rendering/gl/Shader.h"
#include "../Renderer.h"

namespace volumeshOS::Internal
{

    GroundPass::GroundPass()
    {
        auto opt = AppState::settings.ground;
        // Create plane mesh
        m_vao = std::make_unique<VertexArrayObject>(CommonMeshes::PlaneXZ::vertices((float)opt.size, (float)opt.size, 0.0f),
                                                    CommonMeshes::PlaneXZ::indices());
        m_vao->add_attribute(CommonMeshes::PlaneXZ::normals(), 1, 3);
        m_vao->add_attribute(CommonMeshes::PlaneXZ::uvs(), 2, 2);
        m_ground_shader = Shader::get("ground");
        m_pre_ground_shader = Shader::get("pre_ground");
    }

    void GroundPass::render_pre(const Renderer& renderer)
    {
        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);

        m_pre_ground_shader->bind();

        auto& settings = AppState::settings.ground;

        m_pre_ground_shader->set_uniform_mat4f("u_transform", renderer.camera->world);
        m_pre_ground_shader->set_uniform_mat4f("u_projection", renderer.camera->projection);
        m_pre_ground_shader->set_uniform_mat4f("u_view", renderer.camera->view);
        m_pre_ground_shader->set_uniform_float("u_height", settings.height);
        m_pre_ground_shader->set_uniform_bool("u_visible", settings.solid || settings.grid);

        m_vao->draw();

        m_pre_ground_shader->unbind();
    }

    void GroundPass::render(const Renderer& renderer)
    {
        bool shadow_only = renderer.frame.ground_shadow_only;

        renderer.buffers.target_framebuffer_ms->bind();

        auto& settings = AppState::settings;
        auto ground_options = settings.ground;
        auto grid = settings.ground.grid;

        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_TRUE);

        glClear(GL_DEPTH_BUFFER_BIT);

        if (shadow_only)
        {
            glEnable(GL_BLEND);
            glBlendEquation(GL_FUNC_ADD);
            if (renderer.frame.is_rendering_background)
            {
                glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
            }
            else
            {
                glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            }
            grid = false;
        }
        else if (!settings.ground.solid)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBlendEquation(GL_FUNC_ADD);
        }

        m_ground_shader->bind();

        auto cam = renderer.camera;
        auto light = settings.light;

        // Transform
        glm::mat4 transform = cam->world;
        glm::mat4 l_transform = cam->world;
        glm::mat4 view_transform = cam->view * transform;

        glm::vec3 cam_pos(glm::vec4(cam->position, 1.0));
        //glm::vec3 light_pos(data.camera.view * glm::vec4(data.light.light_dir, 1.0));
        glm::mat3 mvp_ti = glm::mat3(glm::transpose(glm::inverse(view_transform)));
        glm::vec3 light_pos(light.direction);
        auto projection = cam->projection;
//        glm::mat4 projection = glm::perspective(glm::radians(cam->zoom),
//                                                (cam->get_viewport_size().x / cam->get_viewport_size().y),
//                                           cam->near,
//                                           cam->far * 2);


        // Shader uniforms
        m_ground_shader->set_uniform_bool("u_wireframe", settings.rendering_mode == RenderingMode::WIREFRAME);
        m_ground_shader->set_uniform_bool("u_vertices", settings.rendering_mode == RenderingMode::ONLY_VERTICES);
        m_ground_shader->set_uniform_bool("u_visible", ground_options.solid || ground_options.grid);
        volumeshOS::log(std::to_string(ground_options.solid || ground_options.grid));
        m_ground_shader->set_uniform_bool("u_solid", ground_options.solid);
        m_ground_shader->set_uniform_vec3f("u_solid_color", ground_options.solid_color);
        m_ground_shader->set_uniform_bool("u_grid", grid);
        m_ground_shader->set_uniform_vec3f("u_grid_color", ground_options.grid_color);
        m_ground_shader->set_uniform_float("u_height", ground_options.height);
        m_ground_shader->set_uniform_int("u_tile_count", ground_options.tiles);

        m_ground_shader->set_uniform_mat4f("u_transform", transform);
        m_ground_shader->set_uniform_mat4f("u_projection", projection);
        m_ground_shader->set_uniform_mat4f("u_view", cam->view);
        m_ground_shader->set_uniform_vec3f("u_light_pos", light_pos);
        m_ground_shader->set_uniform_vec3f("u_cam_pos", cam_pos);
        m_ground_shader->set_uniform_vec3f("u_light_color", light.color);
        m_ground_shader->set_uniform_int("u_cascade_level", settings.num_shadow_cascades - 1);

        m_ground_shader->set_uniform_float("u_spec_strength",0.3f);
        m_ground_shader->set_uniform_float("u_spec_exponent",8.0f);
        m_ground_shader->set_uniform_float("u_ambient_strength",0.9f);
        m_ground_shader->set_uniform_float("u_diffuse_strength", 1.0f);

        m_ground_shader->set_uniform_float("u_shadow_strength", settings.shadow.shadow_strength);

        m_ground_shader->set_uniform_bool("u_use_pbr", ground_options.use_pbr);
        m_ground_shader->set_uniform_float("u_metallic", ground_options.metallic);
        m_ground_shader->set_uniform_float("u_roughness", ground_options.roughness);
        m_ground_shader->set_uniform_float("u_light_intensity", light.intensity);
        m_ground_shader->set_uniform_float("u_gamma", settings.post_processing.gamma);
        m_ground_shader->set_uniform_vec3f("u_background_color", settings.sky.sky_color);

        m_ground_shader->set_uniform_int("u_viewport_width", renderer.frame.width);
        m_ground_shader->set_uniform_int("u_viewport_height", renderer.frame.height);

        m_ground_shader->set_uniform_float("u_near", cam->near);
        m_ground_shader->set_uniform_float("u_far", cam->far);

        float bias_min = 0.000;
        float bias_max = 0.000;
        float bias_modifier = 0.1;

        m_ground_shader->set_uniform_float("u_bias_min", bias_min);
        m_ground_shader->set_uniform_float("u_bias_max", bias_max);
        m_ground_shader->set_uniform_float("u_bias_modifier", bias_modifier);


        // shadow maps
        auto s = renderer.passes.shadow_pass;
        for (int i = 0; i < s->max_cascades; i++)
        {
            m_ground_shader->set_uniform_mat4f("u_light_projection[" + std::to_string(i) + "]",
                                             s->cascade_projections[i]);
            m_ground_shader->set_uniform_mat4f("u_light_view[" + std::to_string(i) + "]", s->cascade_views[i]);
            m_ground_shader->set_uniform_float("u_cascade_ends[" + std::to_string(i) + "]", s->cascade_ends[i]);
        }

        m_ground_shader->set_uniform_mat4f("u_light_transform", l_transform);
        m_ground_shader->set_uniform_float("u_light_size", settings.shadow.penumbra_scale);
        m_ground_shader->set_uniform_float("u_softness", settings.shadow.softness);


        // settings
        m_ground_shader->set_uniform_bool("u_draw_shadows", settings.shadows_active);
        m_ground_shader->set_uniform_bool("u_draw_ao", settings.ssao_active);
        m_ground_shader->set_uniform_bool("u_shadow_only",  renderer.frame.ground_shadow_only);

        // input textures
        m_ground_shader->set_uniform_sampler2D("u_depth_texture", GL_TEXTURE0,
                                             renderer.passes.pre_pass->get_framebuffer()->get_depth_texture());
        m_ground_shader->set_uniform_sampler2D("u_ssao_texture", GL_TEXTURE1,
                                             renderer.passes.ssao_pass->get_blur_texture());

//        // bind cascaded shadow map
//        std::vector<uint32_t> bindings = {GL_TEXTURE4, GL_TEXTURE5, GL_TEXTURE6, GL_TEXTURE7,
//                                              GL_TEXTURE8, GL_TEXTURE9, GL_TEXTURE10, GL_TEXTURE11};
//        for (int i = 0; i < s->max_cascades; i++)
//        {
//            m_ground_shader->set_uniform_sampler2D("u_shadow_texture[" + std::to_string(i) + "]", bindings[i],
//                                                 s->shadow_maps[i]);
//        }

        m_ground_shader->set_uniform_sampler2DArray("u_shadow_texture", GL_TEXTURE4, s->get_depth_texture());

        m_vao->draw();

        m_ground_shader->unbind();
        renderer.buffers.target_framebuffer_ms->unbind();
    }

}