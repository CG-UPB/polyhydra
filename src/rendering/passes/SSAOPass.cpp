
#include "SSAOPass.h"
#include "../meshes/CommonMeshes.h"

namespace volumeshOS::Internal
{
    // best image quality, but also most demanding on the gpu
    const SSAOOptions SSAOPass::QUALITY_SSAO = {
            .active         = true,
            .num_samples    = 64,
            .sample_radius  = 0.5,
            .strength       = 1.5,
            .z_bias         = 0.01
    };

    // balance between image quality and performance
    const SSAOOptions SSAOPass::BALANCED_SSAO = {
            .active         = true,
            .num_samples    = 32,
            .sample_radius  = 0.5,
            .strength       = 1.5,
            .z_bias         = 0.015
    };

    // lowest image quality, but least performance impact
    const SSAOOptions SSAOPass::PERFORMANCE_SSAO = {
            .active         = true,
            .num_samples    = 16,
            .sample_radius  = 0.5,
            .strength       = 1.5,
            .z_bias         = 0.02
    };

    SSAOPass::SSAOPass(int initial_width, int initial_height) :
        m_options(SSAOPass::QUALITY_SSAO)
    {
        // we only need one channel for the occlusion factor
        std::vector<FrameBufferAttachment> ssao_attachments = {
                FrameBufferAttachment{
                        .internal_format    = GL_RED,
                        .format             = GL_RED,
                        .type               = GL_FLOAT,
                        .attachment         = GL_COLOR_ATTACHMENT0,
                        .texture_filter     = GL_NEAREST
                }
        };
        // for the blur factor, we only need one channel as well
        std::vector<FrameBufferAttachment> blur_attachments = {
                FrameBufferAttachment{
                        .internal_format    = GL_RED,
                        .format             = GL_RED,
                        .type               = GL_FLOAT,
                        .attachment         = GL_COLOR_ATTACHMENT0,
                        .texture_filter     = GL_NEAREST
                }
        };

        // create frame buffers and update the shaders we need
        m_ssao_framebuffer = std::make_shared<FrameBufferObject>(initial_width, initial_height, ssao_attachments);
        m_blur_framebuffer = std::make_shared<FrameBufferObject>(initial_width, initial_height, blur_attachments);
        m_ssao_shader = Shader::get("ssao");
        m_ssao_blur_shader = Shader::get("ssao_blur");

        // now generate the data we need to render
        generate_sample_kernel();
        generate_noise_texture();
    }

    SSAOPass::~SSAOPass()
    {
        glDeleteTextures(1, &m_noise_texture);
    }

    float SSAOPass::get_random_float(float min, float max) const
    {
        static std::uniform_real_distribution<float> random_floats(0.0, 1.0);
        static std::default_random_engine generator;
        return lerp(min, max, random_floats(generator));
    }

    float SSAOPass::lerp(float a, float b, float factor) const
    {
        return a + factor * (b - a);
    }

    void SSAOPass::generate_sample_kernel()
    {
        m_sample_kernel.clear();
        while (m_sample_kernel.size() < SSAOPass::s_max_samples)
        {
            // generate random point within range (x: [-1, 1], y: [-1, 1], z: [0, 1])
            glm::vec3 sample(
                    get_random_float(-1.0, 1.0),
                    get_random_float(-1.0, 1.0),
                    get_random_float(0.0, 1.0)
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
            sample *= get_random_float(0.0, 1.0);
            // more samples distributed at the center of the sphere
            float scale = (float) m_sample_kernel.size() / (float) SSAOPass::s_max_samples;
            scale = lerp(0.1, 1.0, std::pow(scale, 2.0f));
            sample *= scale;
            m_sample_kernel.push_back(sample);
        }
    }

    void SSAOPass::generate_noise_texture()
    {
        // generate xy noise between -1 and 1
        std::vector<glm::vec3> noise_data;
        for (size_t i = 0; i < s_noise_size * s_noise_size; i++)
        {
            glm::vec3 noise(
                    get_random_float(-1.0, 1.0),
                    get_random_float(-1.0, 1.0),
                    0.0
            );
            noise_data.push_back(noise);
        }
        // generate opengl texture from noise data
        glGenTextures(1, &m_noise_texture);
        glBindTexture(GL_TEXTURE_2D, m_noise_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, s_noise_size, s_noise_size, 0, GL_RGB, GL_FLOAT, &noise_data[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // set mode to repeat, since we have a very small texture that we want to repeat across the screen
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    void SSAOPass::load_options_from_settings()
    {
        switch (AppState::settings.ssao_mode) {
            case SSAOMode::OFF:
                m_options.active = false;
                break;
            case SSAOMode::QUALITY:
                load_options(SSAOPass::QUALITY_SSAO);
                break;
            case SSAOMode::BALANCED:
                load_options(SSAOPass::BALANCED_SSAO);
                break;
            case SSAOMode::PERFORMANCE:
                load_options(SSAOPass::PERFORMANCE_SSAO);
                break;
            case SSAOMode::CUSTOM:
                load_options(AppState::settings.ssao_custom_options);
                break;
            default:
                return;
        }
    }

    void SSAOPass::render(const Renderer& renderer)
    {
        load_options_from_settings();
        if (m_options.active)
        {
            auto cam = renderer.camera;
            auto light = renderer.light;

            for (const auto& mesh: renderer.render_list)
            {
                auto pre_pass = renderer.passes.pre_pass->get_framebuffer();
                // main ssao pass
                m_ssao_framebuffer->bind();
                glClear(GL_COLOR_BUFFER_BIT);
                m_ssao_shader->bind();
                // options
                m_ssao_shader->set_uniform_int("u_samples", m_options.num_samples);
                m_ssao_shader->set_uniform_float("u_radius", m_options.sample_radius);
                m_ssao_shader->set_uniform_float("u_strength", m_options.strength);
                m_ssao_shader->set_uniform_float("u_bias", m_options.z_bias);
                // ssao related
                m_ssao_shader->set_uniform_vec3f_array("u_sample_kernel", m_sample_kernel);
                m_ssao_shader->set_uniform_sampler2D("u_position", GL_TEXTURE0, pre_pass->get_position_texture());
                m_ssao_shader->set_uniform_sampler2D("u_normal", GL_TEXTURE1, pre_pass->get_normal_texture());
                m_ssao_shader->set_uniform_sampler2D("u_noise", GL_TEXTURE2, m_noise_texture);
                m_ssao_shader->set_uniform_int("u_noise_size", s_noise_size);
                // general
                m_ssao_shader->set_uniform_int("u_viewport_width", renderer.frame.width);
                m_ssao_shader->set_uniform_int("u_viewport_height", renderer.frame.height);
                m_ssao_shader->set_uniform_mat4f("u_projection", cam->projection);
                m_ssao_shader->set_uniform_mat4f("u_view", cam->view);
                m_ssao_shader->set_uniform_float("u_far", cam->far);
                VertexArrayObject::draw_screen_quad();
                m_ssao_shader->unbind();
                m_ssao_framebuffer->unbind();
                // blur pass
                m_blur_framebuffer->bind();
                glClear(GL_COLOR_BUFFER_BIT);
                m_ssao_blur_shader->bind();
                // general
                m_ssao_blur_shader->set_uniform_int("u_viewport_width", renderer.frame.width);
                m_ssao_blur_shader->set_uniform_int("u_viewport_height", renderer.frame.height);
                m_ssao_blur_shader->set_uniform_float("u_far", cam->far);
                // blur related
                m_ssao_blur_shader->set_uniform_sampler2D("u_ssao_input", GL_TEXTURE0, get_ssao_texture());
                m_ssao_blur_shader->set_uniform_sampler2D("u_position", GL_TEXTURE1, pre_pass->get_position_texture());
                m_ssao_blur_shader->set_uniform_int("u_noise_size", s_noise_size);
                VertexArrayObject::draw_screen_quad();
                m_ssao_blur_shader->unbind();
                m_blur_framebuffer->unbind();
            }
        }
        else
        {
            // here we just set the occlusion factor to 1, so it does not change the lighting calculation
            m_blur_framebuffer->bind();
            glClearColor(1.0, 1.0, 1.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT);
            m_blur_framebuffer->unbind();
        }
    }

    void SSAOPass::resize_buffers(int width, int height)
    {
        m_ssao_framebuffer->resize(width, height);
        m_blur_framebuffer->resize(width, height);
    }

    void SSAOPass::load_options(const SSAOOptions& options)
    {
        m_options = options;
    }

    const SSAOOptions& SSAOPass::get_options() const
    {
        return m_options;
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
