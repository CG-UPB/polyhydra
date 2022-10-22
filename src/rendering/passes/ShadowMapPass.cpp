
#include "ShadowMapPass.h"
#include "mesh/MeshProperties.h"

namespace volumeshOS::Internal
{
    ShadowMapPass::ShadowMapPass(int width, int height) : m_width(width), m_height(height)
    {
        m_shadow_shader = Shader::get("shadow_map");
        m_debug_shader = Shader::get("shadow_debug");
        m_debug_framebuffer = std::make_shared<FrameBufferObject>(width, height, FrameBufferObject::RGBA_AND_DEPTH);
        generate_cascade_textures(width, height);
    }

    void ShadowMapPass::generate_cascade_textures(int width, int height)
    {
        glGenFramebuffers(1, &m_shadow_framebuffer);

        glGenTextures(1, &m_depth_texture);
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_depth_texture);
        glTexImage3D(
                GL_TEXTURE_2D_ARRAY,
                0,
                GL_DEPTH_COMPONENT32F,
                width,
                height,
                max_cascades + 1,
                0,
                GL_DEPTH_COMPONENT,
                GL_FLOAT,
                nullptr);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
//        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

        constexpr float border_color[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, border_color);

        glBindFramebuffer(GL_FRAMEBUFFER, m_shadow_framebuffer);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depth_texture, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "Error: " << status << std::endl;
            exit(1);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void ShadowMapPass::render(const Renderer& renderer)
    {
        auto cam = renderer.camera;
        auto& settings = AppState::settings;
        auto light = settings.light;

        if (cam->near != m_current_near || cam->far != m_current_far || settings.num_shadow_cascades != m_current_cascade_level)
        {
            m_current_near = cam->near;
            m_current_far = cam->far;
            m_current_cascade_level = settings.num_shadow_cascades;

        }
        calculate_cascades(renderer);

        m_shadow_shader->bind();

        glBindFramebuffer(GL_FRAMEBUFFER, m_shadow_framebuffer);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_TEXTURE_2D_ARRAY, m_depth_texture, 0);
        glViewport(0, 0, m_width, m_height);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_BACK);  // peter panning


        for (const auto& mesh: renderer.render_list)
        {
            // Transform
            glm::mat4 transform = cam->world * mesh->get_data().get_transform();
            glm::mat4 view_transform = cam->view * transform;

            // Cell operations
            float cell_size = mesh->get_data().cell_size;
            float peel_depth = mesh->get_data().peel_level;
            float slice_depth = mesh->get_data().slice_level;

            auto bb = mesh->get_world_bb(view_transform);
            auto min = bb.first;
            auto max = bb.second;

            // volumeshOS Operations
            glm::vec3 view_dir = -glm::normalize(cam->get_front());
            auto slice_direction = mesh->get_slice_dir(transform, view_dir);

            bool is_bezier_mesh = mesh->is_bezier_mesh();
            // Currently, cells sometimes appear hollow if CULL_FACE is not 
            // disabled for Bézier meshes
            if (is_bezier_mesh)
            {
                glDisable(GL_CULL_FACE);
            }
            else
            {
                glEnable(GL_CULL_FACE);
                glFrontFace(GL_CCW);
                glCullFace(GL_BACK);
            }

            // Shader uniforms
            m_shadow_shader->set_uniform_vec4f("u_object_color", mesh->get_data().color);
            m_shadow_shader->set_uniform_float("u_cell_size", cell_size);
            m_shadow_shader->set_uniform_float("u_peel_depth", peel_depth);
            m_shadow_shader->set_uniform_float("u_max_peel_depth", mesh->get_data().max_peel_depth);
            m_shadow_shader->set_uniform_bool("u_reverse_peeling", mesh->get_data().reverse_peeling);
            m_shadow_shader->set_uniform_float("u_slice_depth", slice_depth);
            m_shadow_shader->set_uniform_vec3f("u_min", min);
            m_shadow_shader->set_uniform_vec3f("u_max", max);
            m_shadow_shader->set_uniform_vec3f("u_slice_direction", slice_direction);
            m_shadow_shader->set_uniform_bool("u_slice_locked", mesh->get_data().slice_locked);
            // Do not use rounding on Bézier meshes.
            m_shadow_shader->set_uniform_bool("u_rounding", (is_bezier_mesh) ? false : mesh->get_data().rounding_active);
            m_shadow_shader->set_uniform_float("u_rounding_size", mesh->get_data().rounding_size);
            m_shadow_shader->set_uniform_float("u_average_cell_size", mesh->get_mvb()->get_average_cell_size());

            for (int i = 0; i < max_cascades; i++)
            {
                auto light_space_mat = cascade_projections[i] * cascade_views[i];
                m_shadow_shader->set_uniform_mat4f("u_light_space_matrices[" + std::to_string(i) + "]" , light_space_mat);
            }
            m_shadow_shader->set_uniform_mat4f("u_light_projection", cascade_projections[0]);
            m_shadow_shader->set_uniform_mat4f("u_transform", transform);

            m_shadow_shader->set_uniform_bool("u_is_bezier_mesh", is_bezier_mesh);
            if(is_bezier_mesh)
            {
                mesh->get_mtb()->bind();
                // Use Bezier Mesh Property to set uniform.
                m_shadow_shader->set_uniform_int("u_bezier_degree", *mesh->get_ovm()->request_mesh_property<int>(MeshProperties::PROP_BEZIER_DEGREE).begin());
                
                // GL_TEXTURE12 is used for control points storage.
                m_shadow_shader->set_uniform_int("u_control_points_tb", 12);
                // Use tessellation level value from toolbar.
                m_shadow_shader->set_uniform_int("u_bezier_tessellation_level", mesh->get_data().tessellation_level);
            }

            auto vao = mesh->get_vao();
            if (mesh->get_data().rounding_active && !is_bezier_mesh)
            {
                vao = mesh->get_mvb()->get_vao_rounded();
            }
            vao->draw_patches();
        }

        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_shadow_shader->unbind();
    }

    void ShadowMapPass::resize_buffers(int width, int height)
    {
        m_width = width;
        m_height = height;
        glDeleteTextures(1, &m_depth_texture);
        glDeleteFramebuffers(1, &m_shadow_framebuffer);
        m_debug_framebuffer->resize(width, height);
        generate_cascade_textures(width, height);
    }

    void ShadowMapPass::calculate_cascades(const Renderer& renderer)
    {
        // calculate near and far plane for each cascade
        for (int i = 1; i <= m_current_cascade_level; i++)
        {
            cascade_ends[i-1] = (float) i / (float)m_current_cascade_level * m_current_far;
        }
//        for (int i = m_current_cascade_level; i >= 1; i--)
//        {
//            cascade_ends[m_current_cascade_level - i] = (float)(1.0f / pow(2, (float)(i - 1))) * m_current_far;
//        }

        // calculate frustum coordiantes for each cascade
        float n;
        float f;
        for (int i = 0; i < m_current_cascade_level; i++)
        {
            if (i == 0)
            {
                n = m_current_near;
                f = cascade_ends[i];
            } else
            {
                n = cascade_ends[i - 1];
                f = cascade_ends[i];
            }
            calculate_cascade(renderer, n, f, i);
        }
    }

    void ShadowMapPass::calculate_cascade(const Renderer& renderer, float near, float far, int i)
    {
        auto cam = renderer.camera;
        auto light = AppState::settings.light;

        //const auto proj = cam.projection;
        const auto proj = glm::perspective(
                (float)glm::radians(cam->zoom),
                (float)m_width / (float)m_height,
                near,
                far
        );

        std::vector<glm::vec4> frustum_corners;
        const auto inverse = glm::inverse(proj * cam->view);

        for (uint32_t x = 0; x < 2; ++x)
        {
            for (uint32_t y = 0; y < 2; ++y)
            {
                for (uint32_t z = 0; z < 2; ++z)
                {
                    glm::vec4 corner = inverse * glm::vec4(2.0f * (float) x - 1.0f, 2.0f * (float) y - 1.0f,
                                                           2.0f * (float) z - 1.0f, 1.0f);
                    frustum_corners.push_back(corner / corner.w);
                }
            }
        }

        glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
        for (auto &c: frustum_corners)
        {
            center += glm::vec3(c);
        }
        center /= frustum_corners.size();

        if(light.direction == glm::vec3(0.0f, 1.0f, 0.0f))
        {
            light.direction = glm::vec3(-0.001f, 1.0f, 0.01f);
        }
        auto light_dir = glm::normalize(light.direction);

        //light.position = center + light_dir;
        light_positions[i] = center + light_dir;
        cascade_views[i] = glm::lookAt(center + light_dir, center, glm::vec3(0.0f, 1.0f, 0.0f));

        float min_x = std::numeric_limits<float>::max();
        float max_x = std::numeric_limits<float>::min();
        float min_y = std::numeric_limits<float>::max();
        float max_y = std::numeric_limits<float>::min();
        float min_z = std::numeric_limits<float>::max();
        float max_z = std::numeric_limits<float>::min();

        for (auto &c: frustum_corners)
        {
            auto transformed_corner = cascade_views[i] * c;
            min_x = std::min(min_x, transformed_corner.x);
            max_x = std::max(max_x, transformed_corner.x);
            min_y = std::min(min_y, transformed_corner.y);
            max_y = std::max(max_y, transformed_corner.y);
            min_z = std::min(min_z, transformed_corner.z);
            max_z = std::max(max_z, transformed_corner.z);
        }

        const float z_mult = 20.0;
        if (min_z < 0)
        {
            min_z *= z_mult;
        } else
        {
            min_z /= z_mult;
        }
        if (max_z < 0)
        {
            max_z /= z_mult;
        } else
        {
            max_z *= z_mult;
        }
        cascade_projections[i] = glm::ortho(min_x, max_x, min_y, max_y, min_z, max_z);
    }

    uint32_t ShadowMapPass::get_depth_texture() const
    {
        return m_depth_texture;
    }

    uint32_t ShadowMapPass::get_debug_texture(const Renderer& renderer, int cascade_level)
    {
        m_debug_shader->bind();
        m_debug_framebuffer->bind();
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_debug_shader->set_uniform_sampler2DArray("u_shadow_texture", GL_TEXTURE0, m_depth_texture);
        m_debug_shader->set_uniform_sampler2D("u_depth", GL_TEXTURE1, renderer.passes.pre_pass->get_framebuffer()->get_depth_texture());
        m_debug_shader->set_uniform_int("u_cascade_level", cascade_level);
        for (int i = 0; i < max_cascades; i++)
        {
            m_debug_shader->set_uniform_mat4f("u_light_projection[" + std::to_string(i) + "]",
                                             cascade_projections[i]);
            m_debug_shader->set_uniform_mat4f("u_light_view[" + std::to_string(i) + "]", cascade_views[i]);
        }
        m_debug_shader->set_uniform_mat4f("u_inv_projection", glm::inverse(renderer.camera->projection));
        VertexArrayObject::draw_screen_quad();
        m_debug_framebuffer->unbind();
        m_debug_shader->unbind();
        return m_debug_framebuffer->get_texture(GL_COLOR_ATTACHMENT0);
    }
}
