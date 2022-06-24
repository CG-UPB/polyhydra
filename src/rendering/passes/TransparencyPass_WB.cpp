
#include "TransparencyPass_WB.h"
#include "../meshes/CommonMeshes.h"

namespace volumeshOS
{
    class MeshView;

    TransparencyPass_WB::TransparencyPass_WB(Renderer* renderer, int width, int height):
            m_renderer(renderer)
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
        generate_transparency_framebuffer(width, height);
    }

    void TransparencyPass_WB::generate_transparency_framebuffer(int width, int height)
    {
        m_transparent_framebuffer->bind();

        m_depth_texture = m_renderer->m_target->get_texture(GL_DEPTH_ATTACHMENT);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_texture, 0);

        glClearBufferfv(GL_COLOR, 0, &m_zeros[0]);
        glClearBufferfv(GL_COLOR, 1, &m_ones[0]);

        m_transparent_framebuffer->unbind();
    }

    void TransparencyPass_WB::clean_up_framebuffer()
    {

    }

    void TransparencyPass_WB::render(std::shared_ptr<VertexArrayObject> vao, const RenderData& data, std::shared_ptr<MeshObject> mesh)
    {
        m_transparency_shader->bind();

        glm::mat4 transform = data.camera.world * mesh->get_data().get_transform();
        glm::mat4 view_transform = data.camera.view * transform;

        // Cell operations
        float cell_size = mesh->get_data().cell_size;
        float peel_depth = mesh->get_data().peel_level;
        float slice_depth = mesh->get_data().slice_level;

        auto bb = mesh->get_world_bb(view_transform);
        auto min = bb.first;
        auto max = bb.second;

        // volumeshOS Operations
        glm::vec3 view_dir = -glm::normalize(data.camera.get_front());
        auto slice_direction = mesh->get_slice_dir(view_transform, view_dir);

        glm::vec3 cam_pos(data.camera.view * glm::vec4(data.camera.position, 1.0));
        glm::vec3 light_pos(data.camera.view * glm::vec4(data.light.light_dir, 1.0));

        bool use_vertex_normals = GlobalViewerSettings::getInstance()->get_mesh_mode() == Phong_Vertexnormals;

        // set all of our uniforms
        m_transparency_shader->set_uniform_mat4f("u_transform", transform);
        m_transparency_shader->set_uniform_mat4f("u_projection", data.camera.projection);
        m_transparency_shader->set_uniform_mat4f("u_view", data.camera.view);
        m_transparency_shader->set_uniform_vec3f("u_light_pos", light_pos);
        m_transparency_shader->set_uniform_vec3f("u_cam_pos", cam_pos);
        m_transparency_shader->set_uniform_vec3f("u_light_color", data.light.color);
        m_transparency_shader->set_uniform_float("u_cell_size", cell_size);
        m_transparency_shader->set_uniform_vec4f("u_object_color", mesh->get_data().color.get_rgba());
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
        m_transparency_shader->set_uniform_int("u_viewport_width", m_renderer->m_target->get_width());
        m_transparency_shader->set_uniform_int("u_viewport_height", m_renderer->m_target->get_height());
        m_transparency_shader->set_uniform_float("u_spec_strength", mesh->get_data().specular_strength);
        m_transparency_shader->set_uniform_float("u_spec_exponent", mesh->get_data().specular_exponent);
        m_transparency_shader->set_uniform_float("u_ambient_strength", mesh->get_data().ambient_strength);
        m_transparency_shader->set_uniform_float("u_diffuse_strength", mesh->get_data().diffuse_strength);
        m_transparency_shader->set_uniform_bool("u_use_vertex_normals", use_vertex_normals);

        vao->draw();
        m_transparency_shader->unbind();
    }

    void TransparencyPass_WB::render_composition()
    {
        m_composite_shader->bind();
        m_composite_shader->set_uniform_sampler2D("accumTexture", GL_TEXTURE0, get_accum_texture());
        m_composite_shader->set_uniform_sampler2D("revealTexture", GL_TEXTURE1, get_reveal_texture());
        VertexArrayObject::draw_screen_quad();
        m_composite_shader->unbind();

    }

    void TransparencyPass_WB::resize_buffers(int width, int height)
    {
        m_transparent_framebuffer->resize(width, height);
        generate_transparency_framebuffer(width, height);
    }

    void TransparencyPass_WB::clear_framebuffer() const
    {
        glClearBufferfv(GL_COLOR, 0, &m_zeros[0]);
        glClearBufferfv(GL_COLOR, 1, &m_ones[0]);
    }

    void TransparencyPass_WB::bind_transparent_buffer()
    {
        m_transparent_framebuffer->bind();
    }

    void TransparencyPass_WB::unbind_transparent_buffer()
    {
        m_transparent_framebuffer->unbind();
    }

    unsigned int TransparencyPass_WB::get_accum_texture()
    {
        return m_transparent_framebuffer->get_texture(GL_COLOR_ATTACHMENT0);
    }

    unsigned int TransparencyPass_WB::get_reveal_texture()
    {
        return m_transparent_framebuffer->get_texture(GL_COLOR_ATTACHMENT1);
    }

}