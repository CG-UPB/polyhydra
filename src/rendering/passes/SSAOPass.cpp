
#include "glad/glad.h"
#include "../meshes/CommonMeshes.h"
#include "SSAOPass.h"

namespace vOS
{
    SSAOPass::SSAOPass(MeshView* mesh_view, int initial_width, int initial_height) : m_mesh_view(mesh_view)
    {
        std::vector<FrameBufferAttachment> ssao_attachments = {
                FrameBufferAttachment{
                        .internal_format    = GL_RED,
                        .format             = GL_RED,
                        .type               = GL_FLOAT,
                        .attachment         = GL_COLOR_ATTACHMENT0,
                        .texture_filter     = GL_NEAREST
                }
        };
        std::vector<FrameBufferAttachment> blur_attachments = {
                FrameBufferAttachment{
                        .internal_format    = GL_RED,
                        .format             = GL_RED,
                        .type               = GL_FLOAT,
                        .attachment         = GL_COLOR_ATTACHMENT0,
                        .texture_filter     = GL_NEAREST
                }
        };
        m_ssao_framebuffer = new FrameBufferObject(initial_width, initial_height, ssao_attachments);
        m_blur_framebuffer = new FrameBufferObject(initial_width, initial_height, blur_attachments);
        m_ssao_shader = Shader::get("ssao");
        m_ssao_blur_shader = Shader::get("ssao_blur");

        std::uniform_real_distribution<float> random_floats(0.0, 1.0);
        std::default_random_engine generator;

        generate_sample_kernel(random_floats, generator);
        generate_noise_texture(random_floats, generator);
    }

    SSAOPass::~SSAOPass()
    {
        delete m_ssao_framebuffer;
        delete m_blur_framebuffer;
        glDeleteTextures(1, &m_noise_texture);
    }

    void SSAOPass::generate_sample_kernel(std::uniform_real_distribution<float>& random_floats,
                                          std::default_random_engine& generator)
    {
        while (m_sample_kernel.size() < SSAOPass::s_max_samples)
        {
            // generate random point within range (x: [-1, 1], y: [-1, 1], z: [0, 1])
            glm::vec3 sample(
                    random_floats(generator) * 2.0 - 1.0,
                    random_floats(generator) * 2.0 - 1.0,
                    random_floats(generator)
            );

            // check if sample is outside the sphere and discard it
            float sample_radius = glm::length(sample);
            if (sample_radius > 1.0)
            {
                continue;
            }

            // normalize point, now it lies on a unit sphere
            sample = glm::normalize(sample);
            // assign random distance from sphere center
            sample *= random_floats(generator);

            // more samples distributed at the center of the sphere
            float scale = (float) m_sample_kernel.size() / SSAOPass::s_max_samples;
            scale = 0.1f + scale * scale * 0.9f;
            sample *= scale;

            m_sample_kernel.push_back(sample);
        }
    }

    void SSAOPass::generate_noise_texture(std::uniform_real_distribution<float>& random_floats,
                                          std::default_random_engine& generator)
    {
        std::vector<glm::vec3> noise_data;
        for (size_t i = 0; i < m_noise_size * m_noise_size; i++)
        {
            glm::vec3 noise(
                    random_floats(generator) * 2.0 - 1.0,
                    random_floats(generator) * 2.0 - 1.0,
                    0.0
            );
            noise_data.push_back(noise);
        }
        // generate opengl texture from noise data
        glGenTextures(1, &m_noise_texture);
        glBindTexture(GL_TEXTURE_2D, m_noise_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_noise_size, m_noise_size, 0, GL_RGB, GL_FLOAT, &noise_data[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    void SSAOPass::render(VertexArrayObject* vao, const RenderData& render_data, int mesh_id)
    {
        if (ImGui::Begin("AO Options"))
        {
            ImGui::SliderInt("num samples", &m_num_samples, 0, SSAOPass::s_max_samples);
            ImGui::SliderFloat("sample radius", &m_sample_radius, 0.0f, 3.0f);
            ImGui::SliderInt("strength", &m_strength, 1, 10);
            ImGui::SliderFloat("z bias", &m_z_bias, 0.0f, 0.1f);
            ImGui::SliderFloat("blur z threshold", &m_blur_z_threshold, 0.0f, 0.2f);
            ImGui::SliderFloat("screen radius (close up performance)", &m_screen_radius, 0.0f, 1.0f, "%.5f", ImGuiSliderFlags_Logarithmic);
            ImGui::End();
        }

        auto pre_pass_framebuffer = m_mesh_view->m_pre_pass->get_framebuffer();

        // main ssao pass
        m_ssao_framebuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT);
        m_ssao_shader->bind();
        m_ssao_shader->set_uniform_float("u_radius", m_sample_radius);
        m_ssao_shader->set_uniform_int("u_strength", m_strength);
        m_ssao_shader->set_uniform_float("u_bias", m_z_bias);
        m_ssao_shader->set_uniform_int("u_samples", m_num_samples);
        m_ssao_shader->set_uniform_float("u_screen_radius", m_screen_radius);
        m_ssao_shader->set_uniform_vec3f_array("u_sample_kernel", m_sample_kernel);
        m_ssao_shader->set_uniform_mat4f("u_projection", render_data.camera.projection);
        m_ssao_shader->set_uniform_mat4f("u_view", render_data.camera.view);
        m_ssao_shader->set_uniform_int("u_viewport_width", m_mesh_view->m_viewportPanelWidth);
        m_ssao_shader->set_uniform_int("u_viewport_height", m_mesh_view->m_viewportPanelHeight);
        m_ssao_shader->set_uniform_sampler2D("u_position", GL_TEXTURE0, pre_pass_framebuffer->get_position_texture());
        m_ssao_shader->set_uniform_sampler2D("u_normal", GL_TEXTURE1, pre_pass_framebuffer->get_normal_texture());
        m_ssao_shader->set_uniform_sampler2D("u_noise", GL_TEXTURE2, m_noise_texture);
        VertexArrayObject::draw_screen_quad();
        m_ssao_shader->unbind();
        m_ssao_framebuffer->unbind();

        // blur pass
        m_blur_framebuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT);
        m_ssao_blur_shader->bind();
        m_ssao_blur_shader->set_uniform_sampler2D("u_ssao_input", GL_TEXTURE0, get_ssao_texture());
        m_ssao_blur_shader->set_uniform_sampler2D("u_position", GL_TEXTURE1, pre_pass_framebuffer->get_position_texture());
        m_ssao_blur_shader->set_uniform_float("u_blur_threshold", m_blur_z_threshold);
        m_ssao_blur_shader->set_uniform_int("u_noise_size", m_noise_size);
        m_ssao_blur_shader->set_uniform_mat4f("u_projection", render_data.camera.projection);
        m_ssao_blur_shader->set_uniform_mat4f("u_view", render_data.camera.view);
        VertexArrayObject::draw_screen_quad();
        m_ssao_blur_shader->unbind();
        m_blur_framebuffer->unbind();
    }

    void SSAOPass::resize_buffers(int width, int height)
    {
        m_ssao_framebuffer->resize(width, height);
        m_blur_framebuffer->resize(width, height);
    }

    unsigned int SSAOPass::get_ssao_texture() const
    {
        return m_ssao_framebuffer->get_texture(GL_COLOR_ATTACHMENT0);
    }

    unsigned int SSAOPass::get_blur_texture() const
    {
        return m_blur_framebuffer->get_texture(GL_COLOR_ATTACHMENT0);
    }
}