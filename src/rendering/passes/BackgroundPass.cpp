
#include "polyhydra/rendering/passes/BackgroundPass.h"

#include "polyhydra/rendering/Renderer.h"
#include "polyhydra/rendering/gl/Shader.h"
#include "polyhydra/rendering/gl/VertexArrayObject.h"

namespace polyhydra::Internal
{

BackgroundPass::BackgroundPass()
{
    m_background_shader = Shader::background_shader();
}

void BackgroundPass::render(const Renderer& renderer)
{
    auto& options = AppState::settings.sky;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    glDepthMask(GL_FALSE);

    renderer.buffers.target_framebuffer_ms->bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_background_shader->bind();
    m_background_shader->set_uniform_vec3f("u_color", options.sky_color);
    m_background_shader->set_uniform_vec3f("u_light_color", AppState::settings.light.color);
    m_background_shader->set_uniform_vec3f("u_light_dir", AppState::settings.light.direction);
    m_background_shader->set_uniform_vec3f("u_view_dir", renderer.camera->get_front());
    m_background_shader->set_uniform_vec3f("u_cam_pos", renderer.camera->position);
    m_background_shader->set_uniform_mat4f("u_inv_projection", glm::inverse(renderer.camera->projection));
    m_background_shader->set_uniform_mat4f("u_inv_view", glm::inverse(renderer.camera->view));
    VertexArrayObject::draw_screen_quad();
    m_background_shader->unbind();

    renderer.buffers.target_framebuffer_ms->unbind();
}
} // namespace polyhydra::Internal