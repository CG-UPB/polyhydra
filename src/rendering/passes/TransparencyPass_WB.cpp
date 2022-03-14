
#include <iostream>
#include "glad/glad.h"
#include "../../Window.h"
#include "TransparencyPass_WB.h"
#include "../../settings/GlobalViewerSettings.h"
#include "../meshes/CommonMeshes.h"


namespace vOS
{
    class MeshView;

    TransparencyPass_WB::TransparencyPass_WB(MeshView *mesh_view, int width, int height):
        m_mesh_view(mesh_view)
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
        m_transparent_framebuffer = new FrameBufferObject(width, height, transparent_attachments);
        generate_transparency_framebuffer(width, height);
    }

    TransparencyPass_WB::~TransparencyPass_WB()
    {
        delete m_transparent_framebuffer;
    }

    void TransparencyPass_WB::generate_transparency_framebuffer(int width, int height)
    {
        m_transparent_framebuffer->bind();

        m_depth_texture = m_mesh_view->m_screen_quad_frameBuffer->get_texture(GL_DEPTH_ATTACHMENT);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_texture, 0);

        glClearBufferfv(GL_COLOR, 0, &m_zeros[0]);
        glClearBufferfv(GL_COLOR, 1, &m_ones[0]);

        m_transparent_framebuffer->unbind();
    }

    void TransparencyPass_WB::clean_up_framebuffer()
    {

    }

    void TransparencyPass_WB::render(VertexArrayObject* vao, const RenderData& data, int mesh_id)
    {
        MeshObject* obj = Window::instance().get_mesh_obj(mesh_id);
        if(obj == nullptr)
            return;

        m_transparency_shader->bind();

        glm::mat4 transform = data.camera.world * obj->get_data().get_transform();
        glm::mat4 view_transform = data.camera.view * transform;

        // Cell operations
        float cell_size = obj->get_data().m_cell_size;
        int peel_depth = obj->get_data().m_peel_level;
        float slice_depth = obj->get_data().m_slice_level;

        auto bb = obj->get_transformed_bb(view_transform);
        auto min = bb.first;
        auto max = bb.second;

        // View Operations
        glm::vec3 view_dir = -glm::normalize(data.camera.get_front());
        auto slice_direction = obj->get_slice_dir(view_transform, view_dir);

        glm::vec3 cam_pos(data.camera.view * glm::vec4(data.camera.position, 1.0));
        glm::vec3 light_pos(data.camera.view * glm::vec4(data.light.position, 1.0));

        bool use_vertex_normals = GlobalViewerSettings::getInstance()->get_mesh_mode() == Phong_Vertexnormals;

        // set all of our uniforms
        m_transparency_shader->set_uniform_mat4f("u_transform", transform);
        m_transparency_shader->set_uniform_mat4f("u_projection", data.camera.projection);
        m_transparency_shader->set_uniform_mat4f("u_view", data.camera.view);
        m_transparency_shader->set_uniform_vec3f("u_light_pos", light_pos);
        m_transparency_shader->set_uniform_vec3f("u_cam_pos", cam_pos);
        m_transparency_shader->set_uniform_vec3f("u_light_color", data.light.color);
        m_transparency_shader->set_uniform_float("u_cell_size", cell_size);
        m_transparency_shader->set_uniform_vec4f("u_object_color", obj->get_data().m_color.get_rgba());
        m_transparency_shader->set_uniform_int("u_peel_depth", peel_depth);
        m_transparency_shader->set_uniform_float("u_slice_depth", slice_depth);
        m_transparency_shader->set_uniform_vec3f("u_min", min);
        m_transparency_shader->set_uniform_vec3f("u_max", max);
        m_transparency_shader->set_uniform_vec3f("u_slice_direction", slice_direction);
        m_transparency_shader->set_uniform_bool("u_slice_locked", obj->get_data().m_slice_locked);
        m_transparency_shader->set_uniform_float("u_pow", m_pow);
        m_transparency_shader->set_uniform_float("u_alpha_pow", m_alpha_pow);
        m_transparency_shader->set_uniform_float("u_range", m_range);
        m_transparency_shader->set_uniform_float("u_depth_range", m_depth_range);
        m_transparency_shader->set_uniform_float("u_ordering_strength", m_ordering_strength);
        m_transparency_shader->set_uniform_float("u_t_min", m_min);
        m_transparency_shader->set_uniform_float("u_t_max", m_max);
        m_transparency_shader->set_uniform_bool("u_rounding", data.rounding.active);
        m_transparency_shader->set_uniform_float("u_rounding_size", data.rounding.size);
        m_transparency_shader->set_uniform_float("u_average_cell_size", obj->get_mvb()->get_average_cell_size());
        m_transparency_shader->set_uniform_int("u_viewport_width", m_mesh_view->m_screen_quad_frameBuffer->get_width());
        m_transparency_shader->set_uniform_int("u_viewport_height", m_mesh_view->m_screen_quad_frameBuffer->get_height());
        m_transparency_shader->set_uniform_float("u_spec_strength", obj->get_data().m_specular_strength);
        m_transparency_shader->set_uniform_float("u_spec_exponent", obj->get_data().m_specular_exponent);
        m_transparency_shader->set_uniform_float("u_ambient_strength", obj->get_data().m_ambient_strength);
        m_transparency_shader->set_uniform_float("u_diffuse_strength", obj->get_data().m_diffuse_strength);
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