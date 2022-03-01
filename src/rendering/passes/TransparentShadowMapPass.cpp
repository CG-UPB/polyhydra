
#include "glad/glad.h"
#include "../../Window.h"
#include "TransparentShadowMapPass.h"

namespace vOS
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
        glDisable(GL_BLEND);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);

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

        vao->draw();

        m_transparent_shadow_shader->unbind();
        m_transparent_shadow_framebuffer->unbind();

        glEnable(GL_CULL_FACE);

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