
#include "panels/Window.h"
#include "TransparentShadowMapPass.h"

namespace volumeshOS::Internal
{
    TransparentShadowMapPass::TransparentShadowMapPass(int width, int height)
    {
        m_transparent_shadow_shader = Shader::get("transparent_shadow_map");

        std::vector<FrameBufferAttachment> attachments =
        {
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
        m_transparent_shadow_framebuffer = std::make_shared<FrameBufferObject>(width, height, attachments);

    }

    void TransparentShadowMapPass::render(std::shared_ptr<VertexArrayObject> vao, const RenderData& data, std::shared_ptr<MeshObject> mesh)
    {
        glDisable( GL_CULL_FACE );
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);

        m_transparent_shadow_framebuffer->bind();
        m_transparent_shadow_shader->bind();

        glm::mat4 light_projection = data.light.projection;
        glm::mat4 light_view = data.light.view;
        glm::mat4 transform = data.camera.world * mesh->get_data().get_transform();
        glm::mat4 l_transform = data.light.world * mesh->get_data().get_transform();

        // Shader uniforms
        m_transparent_shadow_shader->set_uniform_vec4f("u_object_color", mesh->get_data().color.get_rgba());
        m_transparent_shadow_shader->set_uniform_mat4f("u_light_projection", light_projection);
        m_transparent_shadow_shader->set_uniform_mat4f("u_light_view", light_view);
        m_transparent_shadow_shader->set_uniform_mat4f("u_transform", l_transform);

        vao->draw();

        m_transparent_shadow_shader->unbind();
        m_transparent_shadow_framebuffer->unbind();

        glEnable(GL_CULL_FACE);

    }

    void TransparentShadowMapPass::resize_buffers(int width, int height)
    {
        m_transparent_shadow_framebuffer->resize(width, height);
    }

    std::shared_ptr<FrameBufferObject> TransparentShadowMapPass::get_framebuffer() const
    {
        return m_transparent_shadow_framebuffer;
    }

    unsigned int TransparentShadowMapPass::get_shadow_map() const
    {
        return m_transparent_shadow_framebuffer->get_texture(GL_DEPTH_ATTACHMENT);
    }

}