
#include "glad/glad.h"
#include "../../Window.h"
#include "ShadowMapPass.h"

namespace vOS
{
    ShadowMapPass::ShadowMapPass(int width, int height)
    {
        m_shadow_shader = Shader::get("shadow_map");

        std::vector<FrameBufferAttachment> attachments =
        {
                FrameBufferAttachment{
                        .internal_format    = GL_RGBA,
                        .format             = GL_RGBA,
                        .type               = GL_FLOAT,
                        .attachment         = GL_COLOR_ATTACHMENT0,
                        .texture_filter     = GL_NEAREST
                },
                FrameBufferAttachment
                {
                    .internal_format    = GL_DEPTH_COMPONENT,
                    .format             = GL_DEPTH_COMPONENT,
                    .type               = GL_FLOAT,
                    .attachment         = GL_DEPTH_ATTACHMENT,
                    .texture_filter     = GL_NEAREST,
                    .texture_wrap       = GL_REPEAT,
                    .texture_comp_func  = GL_LEQUAL,
                    .texture_comp_mode  = GL_NONE
                }
        };
        m_shadow_framebuffer             = new FrameBufferObject(width, height, attachments);
        //m_shadow_framebuffer_transparent = new FrameBufferObject(width, height, attachments);

    }

    ShadowMapPass::~ShadowMapPass()
    {
        delete m_shadow_framebuffer;
    }

    void ShadowMapPass::render(VertexArrayObject* vao, const RenderData& data, int mesh_id)
    {
        // Get Mesh
        MeshObject* obj = Window::instance().get_mesh_obj(mesh_id);
        if(obj == nullptr)
            return;

        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        m_shadow_framebuffer->bind();
        m_shadow_shader->bind();

        glm::mat4 positionOffset = glm::translate(-obj->get_data().m_offset);
        glm::mat4 light_projection = data.light.projection;
        glm::mat4 light_view = data.light.view;
        glm::mat4 transform = data.light.world * obj->get_data().get_transform() * positionOffset;

        // Shader uniforms
        m_shadow_shader->set_uniform_mat4f("u_light_projection", light_projection);
        m_shadow_shader->set_uniform_mat4f("u_light_view", light_view);
        m_shadow_shader->set_uniform_mat4f("u_transform", transform);

        vao->draw();

        m_shadow_shader->unbind();
        m_shadow_framebuffer->unbind();
    }

    void ShadowMapPass::resize_buffers(int width, int height)
    {
        m_shadow_framebuffer->resize(width, height);
    }

    FrameBufferObject* ShadowMapPass::get_framebuffer() const
    {
        return m_shadow_framebuffer;
    }

    unsigned int ShadowMapPass::get_shadow_map() const
    {
        return m_shadow_framebuffer->get_texture(GL_DEPTH_ATTACHMENT);
    }

}