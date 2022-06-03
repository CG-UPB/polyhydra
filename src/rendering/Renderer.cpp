
#include "Renderer.h"
#include "input/Input.h"

namespace vOS
{

    Renderer::Renderer(int width, int height, FrameBufferObject* initial_target_ms, FrameBufferObject* initial_target) :
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
        last_x = width / 2.0f;
        last_y = height / 2.0f;
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
        m_viewportPanelWidth = width;
        m_viewportPanelHeight = height;
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
        last_x = width / 2.0f;
        last_y = height / 2.0f;
    }

    void Renderer::render(RenderData* render_data, bool render_bg)
    {
        m_render_data = render_data;
        m_is_rendering_background = render_bg;

        m_target_ms->bind();
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_target_ms->unbind();

        // handle input
        handle_camera_input();

        // Render Meshes
        render_pre_pass(*render_data);

        if (m_settings.get_mesh_mode() == ModeEnum::Only_Vertices)
        {
            if (render_bg)
            {
                render_background(*render_data);
            }
            m_target_ms->bind();
            for (const auto& [id, mesh]: Window::instance().get_mesh_list())
            {
                if (mesh == nullptr || !mesh->get_data().visible)
                {
                    continue;
                }
                mesh->update_vertex_buffer();
                if (mesh->get_vao() != nullptr)
                {
                    m_vertex_only_pass.render(nullptr, *render_data, mesh);
                }
            }
            m_target_ms->unbind();
        }
        else
        {
            if (m_settings.get_ambient_occlusion_activated())
            {
                render_ssao_pass(*render_data);
            }

            if (m_settings.get_shadows_activated())
            {
                render_shadow_map(*render_data);
            }


            // Now render our mesh scene to the framebuffer texture
            // Start with opaque objects
            if (render_bg)
            {
                render_background(*render_data);
            }

            render_meshes(*render_data);

            FrameBufferObject::copy(GL_DEPTH_ATTACHMENT, GL_DEPTH_BUFFER_BIT, m_target_ms, m_target);

            // Render transparent objects
            if (m_settings.get_transparency_activated())
            {
                render_transparency(*render_data);
            }

            // Render Selection
            if (m_settings.get_selection_activated())
            {
                render_selection(*render_data);
            }
        }

        // set render states
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE); // enable depth writes so glClear won't ignore clearing the depth buffer
        glDisable(GL_BLEND);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // copy multisampled framebuffer that we rendered on to the imgui texture for display
        FrameBufferObject::copy(GL_COLOR_ATTACHMENT0, GL_COLOR_BUFFER_BIT, m_target_ms, m_target);
    }

    void Renderer::handle_camera_input()
    {
        Input::update();

        auto& cam = m_render_data->camera;

        if (cam.animation)
        {
            cam.animation_step();
            cam.update();
            return;
        }
        // If left ctrl key is pressed, the object move mode is active which requires the camera stand still
        bool ignore_input = Input::key_pressed(GLFW_KEY_LEFT_CONTROL);
        if (ignore_input)
            return;

        if (Input::mouse_double_clicked())
        {
            auto mesh_id = m_selection_hover_pass.get_hovered_mesh_object();

            if (mesh_id >= 0)
            {
                auto mesh = Window::instance().get_mesh_obj(mesh_id);
                glm::vec3 new_target = {0.0f, 0.0f, 0.0f};
                if (Input::key_down(GLFW_KEY_LEFT_CONTROL))
                {
                    auto transform = cam.world * mesh->get_data().get_transform();
                    auto pos_mesh_space = glm::vec4(m_selection_hover_pass.hover_position, 1.0f);
                    //new_target = mesh->get_data().position_offset + glm::vec3(transform * pos_mesh_space);
                    new_target = glm::vec3(transform * pos_mesh_space);
                }
                else
                {
                    new_target = mesh->get_data().position;
                }

                auto extended_target = cam.position + glm::length(glm::vec3(new_target) - cam.position) *
                                                      glm::normalize(cam.target - cam.position);

                if (cam.get_mode() == FLY)
                {
                    cam.look_at(extended_target);
                }
                cam.animated_look_at(new_target);
                cam.set_mode(ORBIT);


                Window::instance().set_mesh_focus(mesh_id);
            }
        }

        if (Input::key_pressed(GLFW_KEY_M))
        {
            auto mesh_id = Window::instance().get_mesh_focus();
            glm::vec3 new_target = cam.target;
            if (mesh_id >= 0)
            {
                auto mesh = Window::instance().get_mesh_obj(mesh_id);
                new_target = mesh->get_data().position;
            }
            cam.switch_mode(new_target);
        }

        ImVec2 vMin = ImGui::GetWindowContentRegionMin();
        ImVec2 vMax = ImGui::GetWindowContentRegionMax();
        vMin.x += ImGui::GetWindowPos().x;
        vMin.y += ImGui::GetWindowPos().y;
        vMax.x += ImGui::GetWindowPos().x;
        vMax.y += ImGui::GetWindowPos().y;

        // mouse movement
        auto mouse_coords = Input::get_mouse_coords();
        auto xpos = mouse_coords.x;
        auto ypos = mouse_coords.y;
        auto is_down = Input::mouse_pressed();

        if (!ImGui::IsWindowHovered() || !ImGui::IsWindowFocused())
        {
            if (!is_down)
            {
                last_x = xpos;
                last_y = ypos;
            }
            return;
        }

        // mouse scroll
        cam.handle_mouse_scroll(Input::get_scroll_offset());

        if (xpos > vMin.x && xpos < vMax.x && ypos > vMin.y && ypos < vMax.y)
        {
            if (is_down)
            {

                float x_offset = xpos - last_x;
                float y_offset = last_y - ypos;

                last_x = xpos;
                last_y = ypos;

                cam.handle_mouse_movement(x_offset, y_offset);
            }
            else
            {
                last_x = xpos;
                last_y = ypos;
            }
        }

        if (ImGui::IsWindowFocused())
        {
            cam.handle_key_movement(Input::get_wasd_movement_vector());
        }

        cam.update();
    }

    void Renderer::render_mesh(RenderData& render_data, const std::shared_ptr<MeshObject>& mesh)
    {
        if (mesh == nullptr)
        {
            return;
        }

        MeshData& mesh_data = mesh->get_data();

        if (!mesh_data.visible)
        {
            return;
        }


        mesh->update_vertex_buffer();

        VertexArrayObject* vao = mesh->get_vao();
        if (mesh_data.rounding_active)
        {
            vao = mesh->get_mvb()->get_vao_rounded();
        }

        // render all passes
        if (vao != nullptr)
        {
            m_mesh_pass->render(vao, render_data, mesh);
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

            for (const auto& [id, mesh]: Window::instance().get_mesh_list())
            {
                if (!mesh->get_data().visible)
                {
                    continue;
                }
                m_selection_pass.render_mesh(mesh, render_data);
            }
        }
        m_selectionFrameBuffer->unbind();

        m_target_ms->bind();
        for (const auto& [id, mesh]: Window::instance().get_mesh_list())
        {
            m_selection_hover_pass.render(nullptr, render_data, mesh);
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
        for (const auto& [id, mesh]: Window::instance().get_mesh_list())
        {
            if (!mesh->get_data().visible)
            {
                continue;
            }
            mesh->update_vertex_buffer();
            VertexArrayObject* vao = mesh->get_vao();
            if (mesh->get_data().rounding_active)
            {
                vao = mesh->get_mvb()->get_vao_rounded();
            }
            if (vao != nullptr)
            {
                m_pre_pass->render(vao, render_data, mesh);
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
        glClearColor(0.0, 0.0, 0.0, 0.0);

        // calculate all cascade matrices
        m_shadow_pass->clear_cascades();
        int cascade_level = GlobalViewerSettings::getInstance()->get_cascade_level();

        auto& cam = render_data.camera;
        m_shadow_pass->calculate_cascades(cam.near, cam.far, cascade_level);

        for (int i = 0; i < cascade_level; i++)
        {
            m_shadow_pass->get_framebuffer()->bind();
            m_shadow_pass->set_cascade_index(i);
            m_shadow_pass->bind_for_writing(i);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            for (const auto& [id, mesh]: Window::instance().get_mesh_list())
            {
                if (!mesh->get_data().visible)
                {
                    continue;
                }
                mesh->update_vertex_buffer();
                VertexArrayObject* vao = mesh->get_vao();
                if (mesh->get_data().rounding_active)
                {
                    vao = mesh->get_mvb()->get_vao_rounded();
                }
                if (vao != nullptr)
                {
                    m_shadow_pass->render(vao, render_data, mesh);
                }
            }
            m_shadow_pass->get_framebuffer()->unbind();
        }
    }

    void Renderer::render_ssao_pass(RenderData& render_data)
    {
        m_ssao_pass->render(nullptr, render_data, nullptr);
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

        for (const auto& [id, mesh]: Window::instance().get_mesh_list())
        {
            MeshData& mesh_data = mesh->get_data();

            if (!mesh->get_data().visible)
            {
                continue;
            }
            mesh->update_vertex_buffer();
            VertexArrayObject* vao = mesh->get_vao();
            if (mesh_data.rounding_active)
            {
                vao = mesh->get_mvb()->get_vao_rounded();
            }
            if (vao != nullptr)
            {
                m_transparency_pass_wb->render(vao, render_data, mesh);
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
            for (const auto& [id, mesh]: Window::instance().get_mesh_list())
            {
                MeshData& mesh_data = mesh->get_data();
                if (!mesh->get_data().visible)
                {
                    continue;
                }

                mesh->update_vertex_buffer();
                VertexArrayObject* vao = mesh->get_vao();
                if (mesh_data.rounding_active)
                {
                    vao = mesh->get_mvb()->get_vao_rounded();
                }

                if (vao != nullptr)
                {
                    m_transparency_pass_dp->render(vao, render_data, mesh, i);
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
        m_background_pass.render(nullptr, render_data, nullptr);
        m_target_ms->unbind();
    }


    void Renderer::render_meshes(RenderData& render_data)
    {
        m_target_ms->bind();
        for (const auto& [id, mesh]: Window::instance().get_mesh_list())
        {
            render_mesh(render_data, mesh);
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