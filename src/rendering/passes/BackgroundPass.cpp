
#include "BackgroundPass.h"
#include "rendering/gl/VertexArrayObject.h"
#include "rendering/gl/Shader.h"
#include "../Renderer.h"

namespace volumeshOS::Internal
{

    BackgroundPass::BackgroundPass()
    {
        m_background_shader = Shader::background_shader();
    }

    void BackgroundPass::render(const Renderer& renderer)
    {
        auto& options = AppState::settings.general;

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDisable(GL_BLEND);
        glDepthMask(GL_FALSE);

        renderer.buffers.target_framebuffer_ms->bind();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_background_shader->bind();
        m_background_shader->set_uniform_vec3f("u_color", options.background_color);
        VertexArrayObject::draw_screen_quad();
        m_background_shader->unbind();

        renderer.buffers.target_framebuffer_ms->unbind();
    }
}