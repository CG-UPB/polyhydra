
#include "glad/glad.h"
#include "../meshes/CommonMeshes.h"
#include "SSAOPass.h"

namespace vOS
{
    SSAOPass::SSAOPass(MeshView* mesh_view, unsigned int initial_width, unsigned int initial_height): m_mesh_view(mesh_view)
    {
        m_ssao_shader = Shader::get("ssao");
        m_ssao_blur_shader = Shader::get("ssao_blur");
        m_screen_quad = new VertexArrayObject(CommonMeshes::PlaneXY::vertices(2.0f, 2.0f), CommonMeshes::PlaneXY::indices());
        m_screen_quad->add_attribute(CommonMeshes::PlaneXY::uvs(), 1, 2);

        generate_framebuffers(initial_width, initial_height);

        std::uniform_real_distribution<float> random_floats(0.0, 1.0);
        std::default_random_engine generator;

        generate_sample_kernel(random_floats, generator);
        generate_noise_texture(random_floats, generator);
    }

    SSAOPass::~SSAOPass()
    {
        clean_up_framebuffers();
        glDeleteTextures(1, &m_noise_texture);
        delete m_screen_quad;
    }

    void SSAOPass::generate_sample_kernel(std::uniform_real_distribution<float>& random_floats, std::default_random_engine& generator)
    {
        for (size_t i = 0; i < SSAOPass::NUM_SAMPLES; i++)
        {
            // generate random point within range (x: [-1, 1], y: [-1, 1], z: [0, 1])
            glm::vec3 sample(
                    random_floats(generator) * 2.0 - 1.0,
                    random_floats(generator) * 2.0 - 1.0,
                    random_floats(generator)
            );
            // normalize point, now it lies on a unit sphere
            sample = glm::normalize(sample);
            // assign random distance from sphere center
            sample *= random_floats(generator);

            // more samples distributed at the center of the sphere
            float scale = (float) i / SSAOPass::NUM_SAMPLES;
            scale = 0.1f + scale * scale * 0.9f;
            sample *= scale;

            m_sample_kernel.push_back(sample);
        }
    }

    void SSAOPass::generate_noise_texture(std::uniform_real_distribution<float>& random_floats, std::default_random_engine& generator)
    {
        const int texture_width = 4;
        const int texture_height = 4;
        std::vector<glm::vec3> noise_data;
        for (size_t i = 0; i < texture_width * texture_height; i++)
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, texture_width, texture_height, 0, GL_RGB, GL_FLOAT, &noise_data[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    void SSAOPass::generate_framebuffers(unsigned int width, unsigned int height)
    {
        m_width = width;
        m_height = height;

        // ssao framebuffer
        glGenFramebuffers(1, &m_ssao_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_ssao_framebuffer);
        // attach ssao texture
        glGenTextures(1, &m_ssao_texture);
        glBindTexture(GL_TEXTURE_2D, m_ssao_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int) m_width, (int) m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ssao_texture, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            fprintf(stderr, "Error when creating ssao framebuffer: %u\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
            return;
        }

        // blur framebuffer
        glGenFramebuffers(1, &m_blur_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_blur_framebuffer);
        // attach blur texture
        glGenTextures(1, &m_blur_texture);
        glBindTexture(GL_TEXTURE_2D, m_blur_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, (int) m_width, (int) m_height, 0, GL_RED, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_blur_texture, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            fprintf(stderr, "Error when creating ssao blur framebuffer: %u\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
            return;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void SSAOPass::clean_up_framebuffers()
    {
        glDeleteFramebuffers(1, &m_ssao_framebuffer);
        glDeleteFramebuffers(1, &m_blur_framebuffer);
        glDeleteTextures(1, &m_ssao_texture);
        glDeleteTextures(1, &m_blur_texture);
    }

    void SSAOPass::render(VertexArrayObject* vao, const RenderData& render_data, int mesh_id)
    {
        if (ImGui::Begin("AO Options"))
        {
            ImGui::SliderInt("samples", &m_uniform_samples, 0, 64);
            ImGui::SliderFloat("radius", &m_uniform_radius, 0.0f, 3.0f);
            ImGui::SliderInt("strength", &m_uniform_strength, 1, 10);
            ImGui::SliderFloat("bias", &m_uniform_bias, 0.0f, 1.0f);
            ImGui::End();
        }

        // remember previous framebuffer, so we can rebind it when we are done
        int previous_framebuffer;
        int previous_viewport[4];
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previous_framebuffer);
        glGetIntegerv(GL_VIEWPORT, previous_viewport);

        // main ssao pass
        glBindFramebuffer(GL_FRAMEBUFFER, m_ssao_framebuffer);
        glViewport(0, 0, (int) m_width, (int) m_height);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_BLEND);
        m_ssao_shader->bind();
        m_ssao_shader->set_uniform_float("u_radius", m_uniform_radius);
        m_ssao_shader->set_uniform_int("u_strength", m_uniform_strength);
        m_ssao_shader->set_uniform_float("u_bias", m_uniform_bias);
        m_ssao_shader->set_uniform_int("u_samples", m_uniform_samples);
        m_ssao_shader->set_uniform_vec3f_array("u_sample_kernel", m_sample_kernel);
        m_ssao_shader->set_uniform_mat4f("u_projection", render_data.camera.projection);
        m_ssao_shader->set_uniform_mat4f("u_view", render_data.camera.view);
        m_ssao_shader->set_uniform_int("u_viewport_width", m_mesh_view->m_screen_quad_frameBuffer->get_width());
        m_ssao_shader->set_uniform_int("u_viewport_height", m_mesh_view->m_screen_quad_frameBuffer->get_height());
        m_ssao_shader->set_uniform_sampler2D("u_depth", GL_TEXTURE0, m_mesh_view->m_pre_pass_framebuffer->get_depth_texture_id());
        m_ssao_shader->set_uniform_sampler2D("u_position", GL_TEXTURE1, m_mesh_view->m_pre_pass_framebuffer->get_position_texture_id());
        m_ssao_shader->set_uniform_sampler2D("u_normal", GL_TEXTURE2, m_mesh_view->m_pre_pass_framebuffer->get_normal_texture_id());
        m_ssao_shader->set_uniform_sampler2D("u_noise", GL_TEXTURE3, m_noise_texture);
        m_screen_quad->draw();
        m_ssao_shader->unbind();

        // blur pass
        glBindFramebuffer(GL_FRAMEBUFFER, m_blur_framebuffer);
        glClear(GL_COLOR_BUFFER_BIT);
        m_ssao_blur_shader->bind();
        m_ssao_blur_shader->set_uniform_sampler2D("u_ssao_input", GL_TEXTURE0, m_ssao_texture);
        m_screen_quad->draw();
        m_ssao_blur_shader->unbind();

        // bind the old framebuffer again
        glBindFramebuffer(GL_FRAMEBUFFER, previous_framebuffer);
        glViewport(previous_viewport[0], previous_viewport[1], previous_viewport[2], previous_viewport[3]);
    }

    void SSAOPass::resize_buffers(unsigned int width, unsigned int height)
    {
        clean_up_framebuffers();
        generate_framebuffers(width, height);
    }

    unsigned int SSAOPass::get_ssao_texture() const
    {
        return m_ssao_texture;
    }
}