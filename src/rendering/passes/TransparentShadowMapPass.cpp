
#include "glad/glad.h"
#include "../../Window.h"
#include "TransparentShadowMapPass.h"

namespace vOS
{
    TransparentShadowMapPass::TransparentShadowMapPass(MeshView *mesh_view, int width, int height):
    m_mesh_view(mesh_view),
    m_width(width),
    m_height(height)
    {
        m_transparent_shadow_shader = Shader::get("transparent_shadow_map");

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
        m_transparent_shadow_framebuffer = new FrameBufferObject(width, height, attachments);

    }

    TransparentShadowMapPass::~TransparentShadowMapPass()
    {
        delete m_transparent_shadow_framebuffer;
    }

    void TransparentShadowMapPass::render(VertexArrayObject* vao, const RenderData& data, int mesh_id)
    {
        // Get Mesh
        MeshObject* obj = Window::instance().get_mesh_obj(mesh_id);
        if(obj == nullptr)
            return;

        glDisable( GL_CULL_FACE );
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        //glBlendFunc(GL_ONE, GL_ONE);
        glBlendEquation(GL_MAX);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_FALSE);

        m_transparent_shadow_framebuffer->bind();
        m_transparent_shadow_shader->bind();

        glm::mat4 positionOffset = glm::translate(-obj->get_data().m_offset);
        glm::mat4 light_projection = data.light.projection;
        glm::mat4 light_view = data.light.view;
        glm::mat4 transform = data.camera.world * obj->get_data().get_transform() * positionOffset;
        glm::mat4 l_transform = data.light.world * obj->get_data().get_transform() * positionOffset;

        // Shader uniforms
        m_transparent_shadow_shader->set_uniform_vec4f("u_object_color", obj->get_data().m_color.get_rgba());
        m_transparent_shadow_shader->set_uniform_mat4f("u_light_projection", light_projection);
        m_transparent_shadow_shader->set_uniform_mat4f("u_light_view", light_view);
        m_transparent_shadow_shader->set_uniform_mat4f("u_transform", l_transform);

        auto opaque_depth = m_mesh_view->m_shadow_pass->get_framebuffer()->get_texture(GL_DEPTH_ATTACHMENT);
        m_transparent_shadow_shader->set_uniform_sampler2D("u_opaque_depth_texture", GL_TEXTURE0, opaque_depth);

        vao->draw();

        m_transparent_shadow_shader->unbind();
        m_transparent_shadow_framebuffer->unbind();

    }

    void TransparentShadowMapPass::resize_buffers(int width, int height)
    {
        m_transparent_shadow_framebuffer->resize(width, height);
    }

    FrameBufferObject* TransparentShadowMapPass::get_framebuffer() const
    {
        return m_transparent_shadow_framebuffer;
    }

    unsigned int TransparentShadowMapPass::get_shadow_map() const
    {
        return m_transparent_shadow_framebuffer->get_texture(GL_DEPTH_ATTACHMENT);
    }

}