
#include <iostream>
#include "glad/glad.h"
#include "../../Window.h"
#include "TransparencyPass_DP.h"
#include "../../settings/GlobalViewerSettings.h"
#include "../meshes/CommonMeshes.h"


namespace vOS
{
    class MeshView;

    TransparencyPass_DP::TransparencyPass_DP(Renderer* renderer, int width,int height):
            m_renderer(renderer),
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
        m_transparent_framebuffer0 = new FrameBufferObject(width, height, transparent_attachments0);

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
        m_transparent_framebuffer1 = new FrameBufferObject(width, height, transparent_attachments1);

        //update_draw_texture();
    }

    TransparencyPass_DP::~TransparencyPass_DP()
    {
        delete m_transparent_framebuffer0;
        delete m_transparent_framebuffer1;

        delete m_vao;

    }

    void TransparencyPass_DP::clean_up_framebuffer()
    {

    }

    void TransparencyPass_DP::render(VertexArrayObject* vao, const RenderData& data, int mesh_id, int pass)
    {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_GREATER);
        glDisable(GL_BLEND);

        if(pass % 2 == 0)
        {
            m_transparent_framebuffer0->bind();
            m_transparency_shader->bind();
            if(pass == 0)
            {
                unsigned int depth_texture = m_renderer->m_target->get_texture(GL_DEPTH_ATTACHMENT);
                m_transparency_shader->set_uniform_sampler2D("last_depth_texture", GL_TEXTURE0, depth_texture);
            }
            else
            {
                unsigned int depth_texture = m_transparent_framebuffer1->get_texture(GL_DEPTH_ATTACHMENT);
                m_transparency_shader->set_uniform_sampler2D("last_depth_texture", GL_TEXTURE0, depth_texture);
            }
            render(vao, data, mesh_id);
            m_transparency_shader->unbind();
            m_transparent_framebuffer0->unbind();
        }
        else
        {
            m_transparent_framebuffer1->bind();
            m_transparency_shader->bind();
            unsigned int depth_texture = m_transparent_framebuffer0->get_texture(GL_DEPTH_ATTACHMENT);
            m_transparency_shader->set_uniform_sampler2D("last_depth_texture", GL_TEXTURE0, depth_texture);
            render(vao, data, mesh_id);
            m_transparency_shader->unbind();
            m_transparent_framebuffer1->unbind();
        }
    }

    void TransparencyPass_DP::render(VertexArrayObject* vao, const RenderData& data, int mesh_id)
    {
        // Get Mesh
        MeshObject* obj = Window::instance().get_mesh_obj(mesh_id);
        if(obj == nullptr)
            return;

        glm::mat4 transform = data.camera.world * obj->get_data().get_transform();
        glm::mat4 view_transform = data.camera.view * transform;

        // Cell operations
        float cell_size = obj->get_data().m_cell_size;
        float peel_depth = obj->get_data().m_peel_level;
        float slice_depth = obj->get_data().m_slice_level;

        auto bb = obj->get_transformed_bb(view_transform);
        auto min = bb.first;
        auto max = bb.second;

        // View Operations
        glm::vec3 view_dir = -glm::normalize(data.camera.get_front());
        auto slice_direction = obj->get_slice_dir(view_transform, view_dir);

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
        m_transparency_shader->set_uniform_vec4f("u_object_color", obj->get_data().m_color.get_rgba());
        m_transparency_shader->set_uniform_float("u_peel_depth", peel_depth);
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
        m_transparency_shader->set_uniform_bool("u_rounding", obj->get_data().m_rounding_activated);
        m_transparency_shader->set_uniform_float("u_rounding_size", obj->get_data().m_rounding_size);
        m_transparency_shader->set_uniform_float("u_average_cell_size", obj->get_mvb()->get_average_cell_size());
        m_transparency_shader->set_uniform_int("u_viewport_width", m_renderer->m_target->get_width());
        m_transparency_shader->set_uniform_int("u_viewport_height", m_renderer->m_target->get_height());
        m_transparency_shader->set_uniform_float("u_spec_strength", obj->get_data().m_specular_strength);
        m_transparency_shader->set_uniform_float("u_spec_exponent", obj->get_data().m_specular_exponent);
        m_transparency_shader->set_uniform_float("u_ambient_strength", obj->get_data().m_ambient_strength);
        m_transparency_shader->set_uniform_float("u_diffuse_strength", obj->get_data().m_diffuse_strength);
        m_transparency_shader->set_uniform_bool("u_use_vertex_normals", use_vertex_normals);

        vao->draw();

    }

    void TransparencyPass_DP::render_composition(int current_pass, int max_passes)
    {
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);

        if (m_renderer->m_is_rendering_background)
        {
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
        }
        else
        {
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        }

        unsigned int new_layer;

        if(current_pass % 2 == 0)
        {
            new_layer = m_transparent_framebuffer0->get_texture(GL_COLOR_ATTACHMENT0);
        }
        else
        {
            new_layer = m_transparent_framebuffer1->get_texture(GL_COLOR_ATTACHMENT0);
        }


        m_renderer->m_target_ms->bind();
        m_composite_shader->bind();
        m_composite_shader->set_uniform_int("u_current_pass", current_pass);
        m_composite_shader->set_uniform_int("u_max_passes", max_passes - 1);

        m_composite_shader->set_uniform_sampler2D("new_layer_texture", GL_TEXTURE0, new_layer);
        VertexArrayObject::draw_screen_quad();
        m_composite_shader->unbind();
        m_renderer->m_target_ms->unbind();


        glDisable(GL_BLEND);
        glClearDepth(1.0f);
        glEnable(GL_CULL_FACE);

    }

    void TransparencyPass_DP::resize_buffers(int width, int height)
    {
        m_transparent_framebuffer0->resize(width, height);
        m_transparent_framebuffer1->resize(width, height);

        //update_draw_texture();
        //generate_transparency_framebuffer(width, height);
    }

    void TransparencyPass_DP::update_draw_texture()
    {
        unsigned int texture = m_renderer->m_target_ms->get_texture(GL_COLOR_ATTACHMENT0);
        m_transparent_framebuffer0->bind();
        m_transparent_framebuffer0->attach_texture(GL_COLOR_ATTACHMENT0, texture);
        m_transparent_framebuffer0->unbind();
        m_transparent_framebuffer1->bind();
        m_transparent_framebuffer1->attach_texture(GL_COLOR_ATTACHMENT0, texture);
        m_transparent_framebuffer1->unbind();
    }

}