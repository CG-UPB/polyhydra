
#include "GroundPass.h"
#include "../meshes/CommonMeshes.h"
#include "rendering/gl/VertexArrayObject.h"
#include "rendering/gl/Shader.h"
#include "../Renderer.h"

namespace volumeshOS::Internal
{

    GroundPass::GroundPass()
    {
        auto opt = AppState::settings.ground_options;
        // Create plane mesh
        m_vao = std::make_unique<VertexArrayObject>(CommonMeshes::PlaneXZ::vertices((float)opt.size, (float)opt.size, 0.0f),
                                                    CommonMeshes::PlaneXZ::indices());
        m_vao->add_attribute(CommonMeshes::PlaneXZ::normals(), 1, 3);
        m_vao->add_attribute(CommonMeshes::PlaneXZ::uvs(), 2, 2);
        m_ground_shader = Shader::get("ground");;
    }


    void GroundPass::render(const Renderer& renderer)
    {
        renderer.buffers.target_framebuffer_ms->bind();

        auto& settings = AppState::settings;
        auto ground_options = settings.ground_options;

        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);

        if(settings.ground_options.grid)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBlendEquation(GL_FUNC_ADD);
        }


        m_ground_shader->bind();

        auto cam = renderer.camera;
        auto light = renderer.light;

        // Transform
        glm::mat4 transform = cam->world;
        glm::mat4 l_transform = light.world;
        glm::mat4 view_transform = cam->view * transform;

        glm::vec3 cam_pos(cam->view * glm::vec4(cam->position, 1.0));
        //glm::vec3 light_pos(data.camera.view * glm::vec4(data.light.light_dir, 1.0));
        glm::mat3 mvp_ti = glm::mat3(glm::transpose(glm::inverse(cam->view)));
        glm::vec3 light_pos(glm::normalize(mvp_ti * light.light_dir));


        // Shader uniforms
        m_ground_shader->set_uniform_bool("u_visible", ground_options.solid || ground_options.grid);
        m_ground_shader->set_uniform_bool("u_solid", ground_options.solid);
        m_ground_shader->set_uniform_vec3f("u_solid_color", ground_options.solid_color);
        m_ground_shader->set_uniform_bool("u_grid", ground_options.grid);
        m_ground_shader->set_uniform_vec3f("u_grid_color", ground_options.grid_color);
        m_ground_shader->set_uniform_float("u_height", ground_options.height);
        m_ground_shader->set_uniform_int("u_tile_count", ground_options.tiles);

        m_ground_shader->set_uniform_mat4f("u_transform", transform);
        m_ground_shader->set_uniform_mat4f("u_projection", cam->projection);
        m_ground_shader->set_uniform_mat4f("u_view", cam->view);
        m_ground_shader->set_uniform_vec3f("u_light_pos", light_pos);
        m_ground_shader->set_uniform_vec3f("u_cam_pos", cam_pos);
        m_ground_shader->set_uniform_vec3f("u_light_color", light.color);
        m_ground_shader->set_uniform_int("u_cascade_level", settings.num_shadow_cascades - 1);
        m_ground_shader->set_uniform_float("u_spec_strength",0.3f);
        m_ground_shader->set_uniform_float("u_spec_exponent",8.0f);
        m_ground_shader->set_uniform_float("u_ambient_strength",0.9f);
        m_ground_shader->set_uniform_float("u_diffuse_strength", 1.0f);

        m_ground_shader->set_uniform_int("u_viewport_width", renderer.frame.width);
        m_ground_shader->set_uniform_int("u_viewport_height", renderer.frame.height);


        float bias_min = 0.000005;
        float bias_max = 0.004;
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


        // settings
        m_ground_shader->set_uniform_bool("u_draw_shadows", settings.shadows_active);
        m_ground_shader->set_uniform_bool("u_draw_ao",  false);

        // input textures
        m_ground_shader->set_uniform_sampler2D("u_depth_texture", GL_TEXTURE0,
                                             renderer.passes.pre_pass->get_framebuffer()->get_depth_texture());
        m_ground_shader->set_uniform_sampler2D("u_ssao_texture", GL_TEXTURE1,
                                             renderer.passes.ssao_pass->get_blur_texture());

        // bind cascaded shadow map
        std::vector<unsigned int> bindings = {GL_TEXTURE4, GL_TEXTURE5, GL_TEXTURE6, GL_TEXTURE7,
                                              GL_TEXTURE8, GL_TEXTURE9, GL_TEXTURE10, GL_TEXTURE11};
        for (int i = 0; i < s->max_cascades; i++)
        {
            m_ground_shader->set_uniform_sampler2D("u_shadow_texture[" + std::to_string(i) + "]", bindings[i],
                                                 s->shadow_maps[i]);
        }

        m_vao->draw();

        m_ground_shader->unbind();
        renderer.buffers.target_framebuffer_ms->unbind();

    }

}