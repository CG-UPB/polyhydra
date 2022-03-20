
#include "glad/glad.h"
#include "../../Window.h"
#include "ShadowMapPass.h"

namespace vOS
{
    ShadowMapPass::ShadowMapPass(MeshView *mesh_view, int width, int height) : m_mesh_view(mesh_view)
    {
        clear_cascades();

        m_shadow_shader = Shader::get("shadow_map");

        std::vector<FrameBufferAttachment> attachments =
                {
                        FrameBufferAttachment{
                                .internal_format    = GL_RGBA,
                                .format             = GL_RGBA,
                                .type               = GL_UNSIGNED_BYTE,
                                .attachment         = GL_COLOR_ATTACHMENT0,
                                .texture_filter     = GL_LINEAR,
                                .texture_wrap       = GL_CLAMP_TO_EDGE
                        }
                };
        m_shadow_framebuffer = new FrameBufferObject(width, height, attachments);
        unsigned int shadow_buffer = m_shadow_framebuffer->get_id();

        for(unsigned int i = 0; i < max_cascades; i++)
        {
            unsigned int tex;
            glGenTextures(1, &tex);
            glBindTexture(GL_TEXTURE_2D, tex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            shadow_maps[i] = tex;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, shadow_buffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_maps[0], 0);

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        if(status != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "Error: " << status << std::endl;
            exit(1);
        }
    }

    ShadowMapPass::~ShadowMapPass()
    {
        delete m_shadow_framebuffer;
    }

    void ShadowMapPass::bind_for_writing(int cascade_idx)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_maps[cascade_idx], 0);
    }

    void ShadowMapPass::render(VertexArrayObject *vao, const RenderData &data, int mesh_id)
    {
        int i = cascade_idx;

        // Get Mesh
        MeshObject *obj = Window::instance().get_mesh_obj(mesh_id);
        if (obj == nullptr)
            return;
        glDisable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_CLAMP);

        m_shadow_shader->bind();


        // Transform
        glm::mat4 light_projection = data.light.projection;
        glm::mat4 light_view = data.light.view;
        glm::mat4 transform = data.camera.world * obj->get_data().get_transform();
        glm::mat4 l_transform = data.light.world * obj->get_data().get_transform();
        glm::mat4 view_transform = data.camera.view * transform;

        // Cell operations
        float cell_size = obj->get_data().m_cell_size;
        float peel_depth = obj->get_data().m_peel_level;
        float slice_depth = obj->get_data().m_slice_level;

        auto bb = obj->get_transformed_bb(view_transform);
        auto min = bb.first;
        auto max = bb.second;

        // View Operations
        glm::vec3 view_dir = -glm::normalize(data.camera.get_front());
        auto slice_direction = obj->get_slice_dir(view_transform, view_dir);

        auto settings = GlobalViewerSettings::getInstance();

        // Shader uniforms
        m_shadow_shader->set_uniform_float("u_cell_size", cell_size);
        m_shadow_shader->set_uniform_float("u_peel_depth", peel_depth);
        m_shadow_shader->set_uniform_float("u_slice_depth", slice_depth);
        m_shadow_shader->set_uniform_vec3f("u_min", min);
        m_shadow_shader->set_uniform_vec3f("u_max", max);
        m_shadow_shader->set_uniform_vec3f("u_slice_direction", slice_direction);
        m_shadow_shader->set_uniform_bool("u_slice_locked", obj->get_data().m_slice_locked);
        m_shadow_shader->set_uniform_bool("u_draw_wireframe", settings->get_mesh_mode() == Wireframe);
        m_shadow_shader->set_uniform_bool("u_rounding", obj->get_data().m_rounding_activated);
        m_shadow_shader->set_uniform_float("u_rounding_size", obj->get_data().m_rounding_size);


//        m_shadow_shader->set_uniform_mat4f("u_light_projection", data.light.projection);
//        m_shadow_shader->set_uniform_mat4f("u_light_view", data.light.projection);
//        m_shadow_shader->set_uniform_mat4f("u_transform", l_transform);
        m_shadow_shader->set_uniform_mat4f("u_light_projection", cascade_projections[i]);
        m_shadow_shader->set_uniform_mat4f("u_light_view", cascade_views[i]);
        m_shadow_shader->set_uniform_mat4f("u_transform", l_transform);

        m_shadow_shader->set_uniform_int("u_viewport_width", m_mesh_view->m_viewportPanelWidth);
        m_shadow_shader->set_uniform_int("u_viewport_height", m_mesh_view->m_viewportPanelHeight);

        vao->draw();

        m_shadow_shader->unbind();

        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);
        glDisable(GL_DEPTH_CLAMP);

    }

    void ShadowMapPass::resize_buffers(int width, int height)
    {
        m_shadow_framebuffer->resize(width, height);
        glDeleteTextures((int) max_cascades,  shadow_maps);
        unsigned int shadow_buffer = m_shadow_framebuffer->get_id();

        for(unsigned int i = 0; i < max_cascades; i++)
        {
            unsigned int tex[1];
            glGenTextures(1, tex);
            glBindTexture(GL_TEXTURE_2D, tex[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            shadow_maps[i] = tex[0];
        }
        glBindFramebuffer(GL_FRAMEBUFFER, shadow_buffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_maps[0], 0);
    }

    FrameBufferObject *ShadowMapPass::get_framebuffer() const
    {
        return m_shadow_framebuffer;
    }

    unsigned int ShadowMapPass::get_shadow_map() const
    {
        return m_shadow_framebuffer->get_texture(GL_DEPTH_ATTACHMENT);
    }

    void ShadowMapPass::calculate_cascades(float near, float far, int cascade_level)
    {
        // calculate near and far plane for each cascade
        for (int i = 1; i <= cascade_level; i++)
        {
            cascade_ends[i-1] = (float) i / (float)cascade_level * far;
        }

        // calculate frustum coordiantes for each cascade
        float n;
        float f;
        for (int i = 0; i < cascade_level; i++)
        {
            if (i == 0)
            {
                n = near;
                f = cascade_ends[i];
            } else
            {
                n = cascade_ends[i - 1];
                f = cascade_ends[i];
            }
            calculate_cascade(n, f, i);
        }
    }

    void ShadowMapPass::calculate_cascade(float near, float far, int i)
    {
        auto& cam = m_mesh_view->m_render_data.camera;
        auto& light = m_mesh_view->m_render_data.light;

        //const auto proj = cam.projection;
        const auto proj = glm::perspective(
                (float) glm::radians(cam.zoom),
                (float) m_mesh_view->m_viewportPanelHeight / (float) m_mesh_view->m_viewportPanelWidth,
                near,
                far
        );


        std::vector<glm::vec4> frustum_corners;
        const auto inverse = glm::inverse(proj * cam.view);


//
//        float ar = (float)m_mesh_view->m_viewportPanelHeight / (float)m_mesh_view->m_viewportPanelWidth;
//
//        float tan_half_hfov = tan(glm::radians(cam.zoom / 2.0f));
//        float tan_half_vfov = tan(glm::radians((cam.zoom * ar) / 2.0f));
//
//        float xn = near * tan_half_hfov;
//        float xf = far * tan_half_hfov;
//        float yn = near * tan_half_vfov;
//        float yf = far * tan_half_vfov;
//
//        frustum_corners.emplace_back(xn, yn, near, 1.0);
//        frustum_corners.emplace_back(-xn, yn, near, 1.0);
//        frustum_corners.emplace_back(xn, -yn, near, 1.0);
//        frustum_corners.emplace_back(-xn, -yn, near, 1.0);
//
//        frustum_corners.emplace_back(xf, yf, far, 1.0);
//        frustum_corners.emplace_back(-xf, yf, far, 1.0);
//        frustum_corners.emplace_back(xf, -yf, far, 1.0);
//        frustum_corners.emplace_back(-xf, -yf, far, 1.0);

        for (unsigned int x = 0; x < 2; ++x)
        {
            for (unsigned int y = 0; y < 2; ++y)
            {
                for (unsigned int z = 0; z < 2; ++z)
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

        auto light_dir = glm::normalize(light.light_dir);
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

        const float z_mult = m_z_mult;
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

    void ShadowMapPass::clear_cascades()
    {
        std::fill_n(cascade_ends, max_cascades, 0.0);
        std::fill_n(cascade_views, max_cascades, glm::mat4(0));
        std::fill_n(cascade_projections, max_cascades, glm::mat4(0));
        std::fill_n(light_positions, max_cascades, glm::vec3(0));
    }
}
