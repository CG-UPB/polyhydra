
#include "glad/glad.h"

#include "Renderer.h"

namespace vOS
{

    Renderer::Renderer(int width, int height, FrameBufferObject* initial_target_ms, FrameBufferObject* initial_target):
        m_viewportPanelWidth(width), m_viewportPanelHeight(height), m_settings(*GlobalViewerSettings::getInstance())
    {
        m_target_ms = initial_target_ms;
        m_target = initial_target;

        m_pre_pass = new PrePass(width, height);
        m_shadow_pass = new ShadowMapPass(this, width * 2, height * 2);
        m_transparent_shadow_pass = new TransparentShadowMapPass(width, height);
        m_shadow_color_filter_pass = new ShadowColorFilterPass(this, width, height);
        m_mesh_pass = new MeshPass(this);
        m_ssao_pass = new SSAOPass(this, width, height);

        m_selectionFrameBuffer = new FrameBufferObject(width / 2, height / 2, FrameBufferObject::RGBA_AND_DEPTH);
        m_pixel_buffer = new PixelBufferObject(2, width / 2, height / 2);

        m_transparency_pass_wb = new TransparencyPass_WB(this, width, height);
        m_transparency_pass_dp = new TransparencyPass_DP(this, width, height);
    }

    Renderer::~Renderer()
    {
        delete m_selectionFrameBuffer;
        delete m_pixel_buffer;
        delete m_pre_pass;
        delete m_mesh_pass;
        delete m_transparency_pass_wb;
        delete m_ssao_pass;
    }

    void Renderer::resize(int width, int height)
    {
        m_viewportPanelWidth = (int) width;
        m_viewportPanelHeight = (int) height;
        m_transparency_pass_wb->resize_buffers(m_viewportPanelWidth, m_viewportPanelHeight);
        m_transparency_pass_dp->resize_buffers(m_viewportPanelWidth, m_viewportPanelHeight);
        m_pre_pass->resize_buffers(m_viewportPanelWidth, m_viewportPanelHeight);
        m_ssao_pass->resize_buffers(m_viewportPanelWidth, m_viewportPanelHeight);
        m_shadow_pass->resize_buffers(m_viewportPanelWidth * 2, m_viewportPanelHeight * 2);
        m_shadow_color_filter_pass->resize_buffers(m_viewportPanelWidth, m_viewportPanelHeight);
        m_transparent_shadow_pass->resize_buffers(m_viewportPanelWidth, m_viewportPanelHeight);
        m_selectionFrameBuffer->resize(m_viewportPanelWidth / 2, m_viewportPanelHeight / 2);
        delete m_pixel_buffer;
        m_pixel_buffer = new PixelBufferObject(2, m_viewportPanelWidth / 2, m_viewportPanelHeight / 2);
    }

    void Renderer::render(RenderData& render_data, bool render_bg)
    {
        m_render_data = render_data;
        m_is_rendering_background = render_bg;

        m_target_ms->bind();
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_target_ms->unbind();

        // Render Meshes
        render_pre_pass(render_data);

        if (m_settings.get_mesh_mode() == ModeEnum::Only_Vertices)
        {
            if (render_bg)
            {
                render_background(render_data);
            }
            m_target_ms->bind();
            for (const std::pair<int, MeshObject*> m: Window::instance().get_mesh_list())
            {
                auto mesh = m.second;
                if (!mesh->get_data().m_visible)
                {
                    continue;
                }
                mesh->update_vertex_buffer();
                if (mesh->get_vao() != nullptr)
                {
                    m_vertex_only_pass.render(nullptr, render_data, m.first);
                }
            }
            m_target_ms->unbind();
        }
        else
        {
            if (m_settings.get_ambient_occlusion_activated())
            {
                render_ssao_pass(render_data);
            }

            if (m_settings.get_shadows_activated())
            {
                render_shadow_map(render_data);
            }


            // Now render our mesh scene to the framebuffer texture
            // Start with opaque objects
            if (render_bg)
            {
                render_background(render_data);
            }

            render_meshes(render_data);

            FrameBufferObject::copy(GL_DEPTH_ATTACHMENT, GL_DEPTH_BUFFER_BIT, m_target_ms,m_target);

            // Render transparent objects
            if (m_settings.get_transparency_activated())
            {
                render_transparency(render_data);
            }

            // Render Selection
            if (m_settings.get_selection_activated())
            {
                render_selection(render_data);
            }
        }

        // set render states
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE); // enable depth writes so glClear won't ignore clearing the depth buffer
        glDisable(GL_BLEND);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // copy multisampled framebuffer that we rendered on to the imgui texture for display
        FrameBufferObject::copy(GL_COLOR_ATTACHMENT0, GL_COLOR_BUFFER_BIT, m_target_ms,m_target);
    }

    void Renderer::render_mesh(RenderData& render_data, int mesh_id)
    {
        // Get Mesh
        MeshObject* obj = Window::instance().get_mesh_obj(mesh_id);
        if (obj == nullptr)
            return;

        MeshData& mesh_data = obj->get_data();

        if (!mesh_data.m_visible)
        {
            return;
        }


        obj->update_vertex_buffer();

        VertexArrayObject* vao = obj->get_vao();
        if (mesh_data.m_rounding_activated)
        {
            vao = obj->get_mvb()->get_vao_rounded();
        }

        // render all passes
        if (vao != nullptr)
        {
            m_mesh_pass->render(vao, render_data, mesh_id);
            //m_shape_pass.render(nullptr, m_render_data, mesh_id);
        }
    }

    void Renderer::render_selection(RenderData& render_data)
    {
        // now render our mesh scene to the framebuffer texture
        m_selectionFrameBuffer->bind();

        // viewport (0,0) starts top left, but framebuffer (0,0) starts bottom left
        // viewport[3] equals viewport height
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        // read Pixel data/color from framebuffer
        ImVec2 mouse_pos_in_window = {
                ImGui::GetMousePos().x - ImGui::GetCursorScreenPos().x - ImGui::GetScrollX(),
                ImGui::GetMousePos().y - ImGui::GetCursorScreenPos().y - ImGui::GetScrollY()
        };
        int x = (int) mouse_pos_in_window.x / 2;
        int y = (int) (viewport[3] * 2 - (int) mouse_pos_in_window.y) / 2;

        GLubyte* data = m_pixel_buffer->start_read(x, y, 1, 1);

        if (data != nullptr)
        {
            // evaluate ID out of color
            int type = data[0] & 3;
            int id;
            if (m_selection_pass.is_debug_mode())
            {
                id = (data[0] + data[1] * 256 + data[2] * 256 * 256) >> 2;
            }
            else
            {
                id = (data[0] + data[1] * 256 + data[2] * 256 * 256 + data[3] * 256 * 256 * 256) >> 2;
            }
            query_selection(type, id);
        }

        m_pixel_buffer->finish_read();

        m_current_frame = (m_current_frame + 1) % m_frame_limit;
        if (m_current_frame == 0)
        {
            // we need to clear our framebuffer as well
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            for (const std::pair<int, MeshObject*> m: Window::instance().get_mesh_list())
            {
                auto mesh = m.second;
                m_selection_pass.render_mesh(mesh, render_data, m.first);
            }
        }
        m_selectionFrameBuffer->unbind();

        m_target_ms->bind();
        for (const std::pair<int, MeshObject*> m: Window::instance().get_mesh_list())
        {
            m_selection_hover_pass.render(nullptr, render_data, m.first);
        }
        m_target_ms->unbind();
    }

    void Renderer::query_selection(int type, int id)
    {
        m_selection_callback(type, id);
    }

    void Renderer::render_pre_pass(RenderData& render_data)
    {
        m_pre_pass->get_framebuffer()->bind();
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_pre_pass->clear_position_buffer(render_data);
        for (const std::pair<int, MeshObject*> m: Window::instance().get_mesh_list())
        {
            auto mesh = m.second;

            if (!mesh->get_data().m_visible)
            {
                continue;
            }
            mesh->update_vertex_buffer();
            VertexArrayObject* vao = mesh->get_vao();
            if (mesh->get_data().m_rounding_activated)
            {
                vao = mesh->get_mvb()->get_vao_rounded();
            }
            if (vao != nullptr)
            {
                m_pre_pass->render(vao, render_data, m.first);
            }
        }
        // we generate a mipmap for the position, this is used for ssao
        // this needs to happen every frame, since the fragment position values always change
        glBindTexture(GL_TEXTURE_2D, m_pre_pass->get_framebuffer()->get_position_texture());
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        m_pre_pass->get_framebuffer()->unbind();
    }

    void Renderer::render_shadow_map(RenderData& render_data)
    {
        // render opaque shadow map
        m_shadow_pass->get_framebuffer()->bind();
        glClearColor(0.0, 0.0, 0.0, 0.0);

        // calculate all cascade matrices
        m_shadow_pass->clear_cascades();
        int cascade_level = 1;
        auto cam = render_data.camera;

        float max = cam.far - cam.near;
        for(int i = 0; i < cascade_level; i++)
        {
            float near = cam.near + (float)i * (max / (float)cascade_level);
            float far = cam.near + (float)(i + 1) * (max / (float)cascade_level);
            m_shadow_pass->calculate_cascade(cam.near, cam.far / 2);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for (const std::pair<int, MeshObject*> m: Window::instance().get_mesh_list())
        {
            auto mesh = m.second;
            if (!mesh->get_data().m_visible)
            {
                continue;
            }
            mesh->update_vertex_buffer();
            VertexArrayObject* vao = mesh->get_vao();
            if (mesh->get_data().m_rounding_activated)
            {
                vao = mesh->get_mvb()->get_vao_rounded();
            }
            if (vao != nullptr)
            {
                m_shadow_pass->render(vao, render_data, m.first);
            }
        }
        m_shadow_color_filter_pass->get_framebuffer()->unbind();
    }

    void Renderer::render_ssao_pass(RenderData& render_data)
    {
        m_ssao_pass->render(nullptr, render_data, -1);
    }

    void Renderer::render_transparency_wb(RenderData& render_data)
    {
        m_transparency_pass_wb->bind_transparent_buffer();
        m_transparency_pass_wb->clear_framebuffer();
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunci(0, GL_ONE, GL_ONE);
        glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
        glBlendEquation(GL_FUNC_ADD);
        //glDisable(GL_CULL_FACE);

        for (const std::pair<int, MeshObject*> m: Window::instance().get_mesh_list())
        {
            auto mesh = m.second;

            MeshData& mesh_data = mesh->get_data();

            if (!mesh->get_data().m_visible)
            {
                continue;
            }
            mesh->update_vertex_buffer();
            VertexArrayObject* vao = mesh->get_vao();
            if (mesh_data.m_rounding_activated)
            {
                vao = mesh->get_mvb()->get_vao_rounded();
            }
            if (vao != nullptr)
            {
                m_transparency_pass_wb->render(vao, render_data, m.first);
            }
        }
        m_transparency_pass_wb->unbind_transparent_buffer();

        glDepthFunc(GL_ALWAYS);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);

        m_target_ms->bind();
        m_transparency_pass_wb->render_composition();
        m_target_ms->unbind();
    }

    void Renderer::render_transparency_dp(RenderData& render_data)
    {
        int num_passes = m_settings.get_number_passes();
        for (int i = 0; i < num_passes; i++)
        {
            if (i % 2 == 0)
            {
                m_transparency_pass_dp->m_transparent_framebuffer0->bind();
            }
            else
            {
                m_transparency_pass_dp->m_transparent_framebuffer1->bind();
            }
            glClearDepth(0.0f);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

            // first render all meshes
            for (const std::pair<int, MeshObject*> m: Window::instance().get_mesh_list())
            {
                auto mesh = m.second;
                MeshData& mesh_data = mesh->get_data();
                if (!mesh->get_data().m_visible)
                {
                    continue;
                }

                mesh->update_vertex_buffer();
                VertexArrayObject* vao = mesh->get_vao();
                if (mesh_data.m_rounding_activated)
                {
                    vao = mesh->get_mvb()->get_vao_rounded();
                }

                if (vao != nullptr)
                {
                    m_transparency_pass_dp->render(vao, render_data, m.first, i);
                }
            }
            if (i % 2 == 0)
            {
                m_transparency_pass_dp->m_transparent_framebuffer0->unbind();
            }
            else
            {
                m_transparency_pass_dp->m_transparent_framebuffer1->unbind();
            }
            m_transparency_pass_dp->render_composition(i, num_passes);
        }
    }


    void Renderer::render_background(RenderData& render_data)
    {

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        m_target_ms->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_background_pass.render(nullptr, render_data, 0);
        m_target_ms->unbind();
    }


    void Renderer::render_meshes(RenderData& render_data)
    {
        m_target_ms->bind();
        for (const auto& m: Window::instance().get_mesh_list())
        {
            render_mesh(render_data, m.first);
        }
        m_target_ms->unbind();
    }


    void Renderer::render_transparency(RenderData& render_data)
    {
        int m_transparency = m_settings.get_transparency_mode();
        switch (m_transparency)
        {
            case DEPTH_PEELING:
                render_transparency_dp(render_data);
                break;
            case WEIGHTED_BLENDED :
                render_transparency_wb(render_data);
            default:
                return;
        }
    }

    void Renderer::set_target_framebuffer(FrameBufferObject* target_ms, FrameBufferObject* target)
    {
        m_target_ms = target_ms;
        m_target = target;
    }
}