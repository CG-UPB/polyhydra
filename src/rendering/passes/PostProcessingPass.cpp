
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
        auto depth_texture = renderer.buffers.target_framebuffer->get_texture(GL_DEPTH_ATTACHMENT);
        renderer.buffers.post_framebuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_post_processing_shader->bind();
        m_post_processing_shader->set_uniform_bool("u_active", AppState::settings.post_processing.active);
        m_post_processing_shader->set_uniform_float("u_saturation", AppState::settings.post_processing.saturation);
        m_post_processing_shader->set_uniform_float("u_contrast", AppState::settings.post_processing.contrast);
        m_post_processing_shader->set_uniform_float("u_gamma", AppState::settings.post_processing.gamma);
        m_post_processing_shader->set_uniform_sampler2D("u_image", GL_TEXTURE0, image_texture);
        m_post_processing_shader->set_uniform_sampler2D("u_depth", GL_TEXTURE1, depth_texture);
        m_post_processing_shader->set_uniform_vec3f("u_cam_pos", renderer.camera->position);
        m_post_processing_shader->set_uniform_mat4f("u_inv_projection", glm::inverse(renderer.camera->projection));
        m_post_processing_shader->set_uniform_float("u_fog_density", AppState::settings.sky.fog_density);
        m_post_processing_shader->set_uniform_vec3f("u_fog_color", AppState::settings.sky.fog_color);
        m_post_processing_shader->set_uniform_vec3f("u_light_color", AppState::settings.light.color);
        m_post_processing_shader->set_uniform_vec3f("u_light_dir", AppState::settings.light.direction);
        VertexArrayObject::draw_screen_quad();
        m_post_processing_shader->unbind();
        renderer.buffers.post_framebuffer->unbind();
    }
}