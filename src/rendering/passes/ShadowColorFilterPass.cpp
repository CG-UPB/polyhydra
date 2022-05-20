
#include "ShadowColorFilterPass.h"

namespace vOS
{
    ShadowColorFilterPass::ShadowColorFilterPass(Renderer* renderer, int width, int height):
    m_renderer(renderer),
    m_width(width),
    m_height(height)
    {
        m_color_filter_shader = Shader::get("shadow_color_filter");


        std::vector<FrameBufferAttachment> attachments =
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
        };
        m_color_filter_framebuffer  = new FrameBufferObject(width, height, attachments);

    }

    ShadowColorFilterPass::~ShadowColorFilterPass()
    {
        delete m_color_filter_framebuffer;
    }

    void ShadowColorFilterPass::render(VertexArrayObject* vao, const RenderData& data, std::shared_ptr<MeshObject> mesh)
    {
        glDisable( GL_CULL_FACE );
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunc( GL_ONE, GL_ONE);
        glBlendEquation(GL_FUNC_ADD);
        glDepthFunc(GL_LESS);


        m_color_filter_framebuffer->bind();
        m_color_filter_shader->bind();

        glm::mat4 light_projection = data.light.projection;
        glm::mat4 light_view = data.light.view;
        glm::mat4 transform = data.camera.world * mesh->get_data().get_transform();
        glm::mat4 l_transform = data.light.world * mesh->get_data().get_transform();

        // Shader uniforms
        m_color_filter_shader->set_uniform_vec4f("u_object_color", mesh->get_data().color.get_rgba());
        m_color_filter_shader->set_uniform_mat4f("u_light_projection", light_projection);
        m_color_filter_shader->set_uniform_mat4f("u_light_view", light_view);
        m_color_filter_shader->set_uniform_mat4f("u_transform", l_transform);

        auto opaque_depth = m_renderer->m_shadow_pass->get_framebuffer()->get_texture(GL_DEPTH_ATTACHMENT);
        m_color_filter_shader->set_uniform_sampler2D("u_opaque_depth_texture", GL_TEXTURE0, opaque_depth);

        vao->draw();

        m_color_filter_shader->unbind();
        m_color_filter_framebuffer->unbind();
    }

    void ShadowColorFilterPass::resize_buffers(int width, int height)
    {
        m_color_filter_framebuffer->resize(width, height);
    }

    FrameBufferObject* ShadowColorFilterPass::get_framebuffer() const
    {
        return m_color_filter_framebuffer;
    }

    unsigned int ShadowColorFilterPass::get_shadow_map() const
    {
        return m_color_filter_framebuffer->get_texture(GL_DEPTH_ATTACHMENT);
    }

}