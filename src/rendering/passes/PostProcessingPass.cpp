
#include "PostProcessingPass.h"
#include "rendering/gl/VertexArrayObject.h"
#include "rendering/gl/Shader.h"
#include "../Renderer.h"

namespace volumeshOS::Internal
{
    PostProcessingPass::PostProcessingPass()
    {
        m_post_processing_shader = Shader::get("post_processing");
    }

    void PostProcessingPass::render(const Renderer& renderer)
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        auto image_texture = renderer.buffers.target_framebuffer->get_texture(GL_COLOR_ATTACHMENT0);
        renderer.buffers.post_framebuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_post_processing_shader->bind();
        m_post_processing_shader->set_uniform_float("u_gamma", AppState::settings.general.gamma);
        m_post_processing_shader->set_uniform_sampler2D("u_image", GL_TEXTURE0, image_texture);
        VertexArrayObject::draw_screen_quad();
        m_post_processing_shader->unbind();
        renderer.buffers.post_framebuffer->unbind();
    }
}