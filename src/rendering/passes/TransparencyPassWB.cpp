
#include "TransparencyPassWB.h"
#include "../meshes/CommonMeshes.h"

namespace volumeshOS::Internal
{
    class MeshView;

    TransparencyPassWB::TransparencyPassWB(const Renderer& renderer, int width, int height)
    {
        m_transparency_shader = Shader::get("transparency_wb");
        m_composite_shader = Shader::get("composite_wb");

        std::vector<FrameBufferAttachment> transparent_attachments = {
                FrameBufferAttachment{
                        .internal_format    = GL_RGBA32F,
                        .format             = GL_RGBA,
                        .type               = GL_HALF_FLOAT,
                        .attachment         = GL_COLOR_ATTACHMENT0,
                        .texture_filter     = GL_LINEAR,
                        .texture_wrap       = GL_CLAMP_TO_EDGE
                },
                FrameBufferAttachment{
                        .internal_format    = GL_R16,
                        .format             = GL_RED,
                        .type               = GL_FLOAT,
                        .attachment         = GL_COLOR_ATTACHMENT1,
                        .texture_filter     = GL_LINEAR,
                        .texture_wrap       = GL_CLAMP_TO_EDGE
                }
        };
        m_transparent_framebuffer = std::make_shared<FrameBufferObject>(width, height, transparent_attachments);
        generate_transparency_framebuffer(renderer);
    }

    void TransparencyPassWB::generate_transparency_framebuffer(const Renderer& renderer)
    {
        m_transparent_framebuffer->bind();

        m_depth_texture = renderer.buffers.target_framebuffer->get_texture(GL_DEPTH_ATTACHMENT);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_texture, 0);

        glClearBufferfv(GL_COLOR, 0, &m_zeros[0]);
        glClearBufferfv(GL_COLOR, 1, &m_ones[0]);

        m_transparent_framebuffer->unbind();
    }

    void TransparencyPassWB::clean_up_framebuffer()
    {

    }

    void TransparencyPassWB::render(const Renderer& renderer)
    {
        m_transparent_framebuffer->bind();
        clear_framebuffer();

        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunci(0, GL_ONE, GL_ONE);
        glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
        glBlendEquation(GL_FUNC_ADD);

        auto cam = renderer.camera;
        auto light = renderer.light;


        for (const auto& mesh: renderer.render_list)
        {
            m_transparency_shader->bind();

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
            m_transparency_shader->set_uniform_int("u_viewport_width", renderer.buffers.target_framebuffer_ms->get_width());
            m_transparency_shader->set_uniform_int("u_viewport_height", renderer.buffers.target_framebuffer_ms->get_height());
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
            m_transparency_shader->unbind();
        }
        m_transparent_framebuffer->unbind();

        glDepthFunc(GL_ALWAYS);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);

        renderer.buffers.target_framebuffer_ms->bind();
        render_composition();
        renderer.buffers.target_framebuffer_ms->unbind();

//        glDisable(GL_BLEND);
//        glClearDepth(1.0f);
//        glEnable(GL_CULL_FACE);
    }

    void TransparencyPassWB::render_composition()
    {
        glEnable(GL_FRAMEBUFFER_SRGB);
        m_composite_shader->bind();
        m_composite_shader->set_uniform_float("u_gamma", AppState::settings.gamma);
        m_composite_shader->set_uniform_sampler2D("accumTexture", GL_TEXTURE0, get_accum_texture());
        m_composite_shader->set_uniform_sampler2D("revealTexture", GL_TEXTURE1, get_reveal_texture());
        VertexArrayObject::draw_screen_quad();
        m_composite_shader->unbind();
        glDisable(GL_FRAMEBUFFER_SRGB);
    }

    void TransparencyPassWB::resize_buffers(const Renderer& renderer, int width, int height)
    {
        m_transparent_framebuffer->resize(width, height);
        generate_transparency_framebuffer(renderer);
    }

    void TransparencyPassWB::clear_framebuffer() const
    {
        glClearBufferfv(GL_COLOR, 0, &m_zeros[0]);
        glClearBufferfv(GL_COLOR, 1, &m_ones[0]);
    }

    uint32_t TransparencyPassWB::get_accum_texture()
    {
        return m_transparent_framebuffer->get_texture(GL_COLOR_ATTACHMENT0);
    }

    uint32_t TransparencyPassWB::get_reveal_texture()
    {
        return m_transparent_framebuffer->get_texture(GL_COLOR_ATTACHMENT1);
    }

}