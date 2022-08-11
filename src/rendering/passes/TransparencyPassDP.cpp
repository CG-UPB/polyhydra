
#include "TransparencyPassDP.h"
#include "../meshes/CommonMeshes.h"
#include "../../settings/AppState.h"

namespace volumeshOS::Internal
{
    class MeshView;

    TransparencyPassDP::TransparencyPassDP(int width, int height) :
            m_width(width),
            m_height(height)
    {
        m_transparency_shader = Shader::get("transparency_dp");
        m_composite_shader = Shader::get("composite_dp");

        std::vector<FrameBufferAttachment> transparent_attachments0 =
                {
                        FrameBufferAttachment
                                {
                                        .internal_format    = GL_RGBA16F,
                                        .format             = GL_RGBA,
                                        .type               = GL_FLOAT,
                                        .attachment         = GL_COLOR_ATTACHMENT0,
                                        .texture_filter     = GL_LINEAR,
                                        .texture_wrap       = GL_CLAMP_TO_EDGE
                                },
                        FrameBufferAttachment
                                {
                                        .internal_format    = GL_DEPTH_COMPONENT,
                                        .format             = GL_DEPTH_COMPONENT,
                                        .type               = GL_FLOAT,
                                        .attachment         = GL_DEPTH_ATTACHMENT,
                                        .texture_filter     = GL_NEAREST,
                                        .texture_wrap       = GL_CLAMP_TO_EDGE,
                                        .texture_comp_func  = GL_LEQUAL,
                                        .texture_comp_mode  = GL_NONE
                                }
                };
        m_transparent_framebuffer0 = std::make_shared<FrameBufferObject>(width, height, transparent_attachments0);

        std::vector<FrameBufferAttachment> transparent_attachments1 =
                {
                        FrameBufferAttachment{
                                .internal_format    = GL_RGBA16F,
                                .format             = GL_RGBA,
                                .type               = GL_FLOAT,
                                .attachment         = GL_COLOR_ATTACHMENT0,
                                .texture_filter     = GL_LINEAR,
                                .texture_wrap       = GL_CLAMP_TO_EDGE

                        },
                        FrameBufferAttachment
                                {
                                        .internal_format    = GL_DEPTH_COMPONENT,
                                        .format             = GL_DEPTH_COMPONENT,
                                        .type               = GL_FLOAT,
                                        .attachment         = GL_DEPTH_ATTACHMENT,
                                        .texture_filter     = GL_NEAREST,
                                        .texture_wrap       = GL_CLAMP_TO_EDGE,
                                        .texture_comp_func  = GL_LEQUAL,
                                        .texture_comp_mode  = GL_NONE
                                }
                };
        m_transparent_framebuffer1 = std::make_shared<FrameBufferObject>(width, height, transparent_attachments1);

        //update_draw_texture();
    }

    void TransparencyPassDP::render(const Renderer& renderer)
    {
        auto& settings = AppState::settings;
        int num_passes = settings.num_depth_peeling_passes;
        for (int i = 0; i < num_passes; i++)
        {
            if (i % 2 == 0)
            {
                m_transparent_framebuffer0->bind();
            }
            else
            {
                m_transparent_framebuffer1->bind();
            }
            glClearDepth(0.0f);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


            for (const auto& mesh: renderer.render_list)
            {
                if (i % 2 == 0)
                {
                    m_transparent_framebuffer0->bind();
                    m_transparency_shader->bind();
                    if (i == 0)
                    {
                        uint32_t depth_texture = renderer.buffers.target_framebuffer->get_texture(
                                GL_DEPTH_ATTACHMENT);
                        m_transparency_shader->set_uniform_sampler2D("last_depth_texture", GL_TEXTURE0, depth_texture);
                    }
                    else
                    {
                        uint32_t depth_texture = m_transparent_framebuffer1->get_texture(GL_DEPTH_ATTACHMENT);
                        m_transparency_shader->set_uniform_sampler2D("last_depth_texture", GL_TEXTURE0, depth_texture);
                    }
                    render_mesh(renderer, mesh);
                    m_transparency_shader->unbind();
                    m_transparent_framebuffer0->unbind();
                }
                else
                {
                    m_transparent_framebuffer1->bind();
                    m_transparency_shader->bind();
                    uint32_t depth_texture = m_transparent_framebuffer0->get_texture(GL_DEPTH_ATTACHMENT);
                    m_transparency_shader->set_uniform_sampler2D("last_depth_texture", GL_TEXTURE0, depth_texture);
                    render_mesh(renderer, mesh);
                    m_transparency_shader->unbind();
                    m_transparent_framebuffer1->unbind();
                }
            }

            if (i % 2 == 0)
            {
                m_transparent_framebuffer0->unbind();
            }
            else
            {
                m_transparent_framebuffer1->unbind();
            }
            render_composition(renderer, i, num_passes);
        }
    }

    void TransparencyPassDP::render_mesh(const Renderer& renderer, const std::shared_ptr<MeshObject>& mesh)
    {
        auto cam = renderer.camera;
        auto light = renderer.light;


        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_GREATER);
        glDisable(GL_BLEND);

        glm::mat4 transform = cam->world * mesh->get_data().get_transform();
        glm::mat4 view_transform = cam->view * transform;

        // Cell operations
        float cell_size = mesh->get_data().cell_size;
        float peel_depth = mesh->get_data().peel_level;
        float slice_depth = mesh->get_data().slice_level;

        auto bb = mesh->get_world_bb(view_transform);
        auto min = bb.first;
        auto max = bb.second;

        // volumeshOS Operations
        glm::vec3 view_dir = -glm::normalize(cam->get_front());
        auto slice_direction = mesh->get_slice_dir(transform, view_dir);

        glm::vec3 cam_pos(cam->view * glm::vec4(cam->position, 1.0));
        glm::vec3 light_pos(cam->view * glm::vec4(light.light_dir, 1.0));

        bool use_vertex_normals = AppState::settings.rendering_mode == RenderingMode::PHONG_VERTEX_NORMALS;

        // set all of our uniforms
        m_transparency_shader->set_uniform_mat4f("u_transform", transform);
        m_transparency_shader->set_uniform_mat4f("u_projection", cam->projection);
        m_transparency_shader->set_uniform_mat4f("u_view", cam->view);
        m_transparency_shader->set_uniform_vec3f("u_light_pos", light_pos);
        m_transparency_shader->set_uniform_vec3f("u_cam_pos", cam_pos);
        m_transparency_shader->set_uniform_vec3f("u_light_color", light.color);
        m_transparency_shader->set_uniform_float("u_cell_size", cell_size);
        m_transparency_shader->set_uniform_vec4f("u_object_color", mesh->get_data().color);
        m_transparency_shader->set_uniform_float("u_peel_depth", peel_depth);
        m_transparency_shader->set_uniform_float("u_slice_depth", slice_depth);
        m_transparency_shader->set_uniform_vec3f("u_min", min);
        m_transparency_shader->set_uniform_vec3f("u_max", max);
        m_transparency_shader->set_uniform_vec3f("u_slice_direction", slice_direction);
        m_transparency_shader->set_uniform_bool("u_slice_locked", mesh->get_data().slice_locked);
        m_transparency_shader->set_uniform_float("u_pow", m_pow);
        m_transparency_shader->set_uniform_float("u_alpha_pow", m_alpha_pow);
        m_transparency_shader->set_uniform_float("u_range", m_range);
        m_transparency_shader->set_uniform_float("u_depth_range", m_depth_range);
        m_transparency_shader->set_uniform_float("u_ordering_strength", m_ordering_strength);
        m_transparency_shader->set_uniform_float("u_t_min", m_min);
        m_transparency_shader->set_uniform_float("u_t_max", m_max);
        m_transparency_shader->set_uniform_bool("u_rounding", mesh->get_data().rounding_active);
        m_transparency_shader->set_uniform_float("u_rounding_size", mesh->get_data().rounding_size);
        m_transparency_shader->set_uniform_float("u_average_cell_size", mesh->get_mvb()->get_average_cell_size());
        m_transparency_shader->set_uniform_int("u_viewport_width", renderer.buffers.target_framebuffer->get_width());
        m_transparency_shader->set_uniform_int("u_viewport_height", renderer.buffers.target_framebuffer->get_height());
        m_transparency_shader->set_uniform_float("u_spec_strength", mesh->get_data().specular_strength);
        m_transparency_shader->set_uniform_float("u_spec_exponent", mesh->get_data().specular_exponent);
        m_transparency_shader->set_uniform_float("u_ambient_strength", mesh->get_data().ambient_strength);
        m_transparency_shader->set_uniform_float("u_diffuse_strength", mesh->get_data().diffuse_strength);
        m_transparency_shader->set_uniform_bool("u_use_vertex_normals", use_vertex_normals);

        auto vao = mesh->get_vao();
        if (mesh->get_data().rounding_active)
        {
            vao = mesh->get_mvb()->get_vao_rounded();
        }
        vao->draw();

    }

    void TransparencyPassDP::render_composition(const Renderer& renderer, int current_pass, int max_passes)
    {
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glEnable(GL_FRAMEBUFFER_SRGB);

        if (renderer.frame.is_rendering_background)
        {
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
        }
        else
        {
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        }

        uint32_t new_layer;

        if (current_pass % 2 == 0)
        {
            new_layer = m_transparent_framebuffer0->get_texture(GL_COLOR_ATTACHMENT0);
        }
        else
        {
            new_layer = m_transparent_framebuffer1->get_texture(GL_COLOR_ATTACHMENT0);
        }


        renderer.buffers.target_framebuffer_ms->bind();
        m_composite_shader->bind();
        m_composite_shader->set_uniform_float("u_gamma", AppState::settings.gamma);
        m_composite_shader->set_uniform_int("u_current_pass", current_pass);
        m_composite_shader->set_uniform_int("u_max_passes", max_passes - 1);

        m_composite_shader->set_uniform_sampler2D("new_layer_texture", GL_TEXTURE0, new_layer);
        VertexArrayObject::draw_screen_quad();
        m_composite_shader->unbind();
        renderer.buffers.target_framebuffer_ms->unbind();


        glDisable(GL_BLEND);
        glClearDepth(1.0f);
        glEnable(GL_CULL_FACE);
        glDisable(GL_FRAMEBUFFER_SRGB);
    }

    void TransparencyPassDP::resize_buffers(int width, int height)
    {
        m_transparent_framebuffer0->resize(width, height);
        m_transparent_framebuffer1->resize(width, height);

        //update_draw_texture();
        //generate_transparency_framebuffer(width, height);
    }

    void TransparencyPassDP::update_draw_texture()
    {
        //uint32_t texture = renderer.buffers.target_framebuffer_ms->get_texture(GL_COLOR_ATTACHMENT0);
//        m_transparent_framebuffer0->bind();
//        m_transparent_framebuffer0->attach_texture(GL_COLOR_ATTACHMENT0, texture);
//        m_transparent_framebuffer0->unbind();
//        m_transparent_framebuffer1->bind();
//        m_transparent_framebuffer1->attach_texture(GL_COLOR_ATTACHMENT0, texture);
//        m_transparent_framebuffer1->unbind();
    }

}