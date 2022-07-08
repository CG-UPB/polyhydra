
#include "Renderer.h"

#include <utility>
#include "input/Input.h"

namespace volumeshOS::Internal
{

    Renderer::Renderer(
            int width,
            int height,
            const std::shared_ptr<FrameBufferObject>& initial_target_ms,
            const std::shared_ptr<FrameBufferObject>& initial_target
            ): m_settings(*GlobalViewerSettings::getInstance())
    {
        frame.width = width;
        frame.height = height;

        buffers.target_framebuffer_ms = initial_target_ms;
        buffers.target_framebuffer = initial_target;
        buffers.selection_frame_buffer = std::make_shared<FrameBufferObject>(width / 2, height / 2, FrameBufferObject::RGBA_AND_DEPTH);
        buffers.pixel_buffer = std::make_shared<PixelBufferObject>(2, width / 2, height / 2);

        passes.background_pass = std::make_shared<BackgroundPass>();
        passes.pre_pass = std::make_shared<PrePass>(width, height);
        passes.shadow_pass = std::make_shared<ShadowMapPass>(this, width * 2, height * 2);
        passes.mesh_pass = std::make_shared<MeshPass>(this);
        passes.ssao_pass = std::make_shared<SSAOPass>(this, width, height);
        passes.transparency_pass_wb = std::make_shared<TransparencyPassWB>(this, width, height);
        passes.transparency_pass_dp = std::make_shared<TransparencyPassDP>(this, width, height);
        passes.shape_pass = std::make_shared<ShapePass>();
        passes.selection_pass = std::make_shared<SelectionPass>();
        passes.selection_hover_pass = std::make_shared<SelectionHoverPass>();
        passes.vertex_only_pass = std::make_shared<VertexOnlyPass>();

        input.last.x = (float) width / 2.0f;
        input.last.y = (float) height / 2.0f;

        mesh_list = std::make_shared<MeshList>();
        camera = std::make_shared<Camera>();

    }

    void Renderer::resize(int width, int height)
    {
        frame.width = width;
        frame.height = height;
        passes.transparency_pass_wb->resize_buffers(frame.width, frame.height);
        passes.transparency_pass_dp->resize_buffers(frame.width, frame.height);
        passes.pre_pass->resize_buffers(frame.width, frame.height);
        passes.ssao_pass->resize_buffers(frame.width, frame.height);
        passes.shadow_pass->resize_buffers(frame.width * 2, frame.height * 2);
        buffers.selection_frame_buffer->resize(frame.width / 2, frame.height / 2);
        buffers.pixel_buffer = std::make_shared<PixelBufferObject>(2, frame.width / 2, frame.height / 2);
        input.last.x = (float) width / 2.0f;
        input.last.y = (float) height / 2.0f;
    }

    void Renderer::render(bool render_bg)
    {
        frame.is_rendering_background = render_bg;

        // handle input
        handle_input();

        render_list.clear();
        mesh_list->iterate([&](auto id, auto mesh){
            if(mesh->get_data().visible)
            {
                mesh->update_vertex_buffer();
                if (mesh->get_vao() != nullptr)
                {
                    render_list.push_back(mesh);
                }
            }
        });

        buffers.target_framebuffer_ms->bind();
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        buffers.target_framebuffer_ms->unbind();

        // Render Meshes
        passes.pre_pass->render(this);

        if (m_settings.get_mesh_mode() == ModeEnum::Only_Vertices)
        {
            if (render_bg)
            {
                passes.background_pass->render(this);
            }
            passes.vertex_only_pass->render(this);
        }
        else
        {
            if (m_settings.get_ambient_occlusion_activated())
            {
                passes.ssao_pass->render(this);
            }

            if (m_settings.get_shadows_activated())
            {
                passes.shadow_pass->render(this);
            }

            if (render_bg)
            {
                passes.background_pass->render(this);
            }

            passes.mesh_pass->render(this);

            FrameBufferObject::copy(GL_DEPTH_ATTACHMENT, GL_DEPTH_BUFFER_BIT, buffers.target_framebuffer_ms, buffers.target_framebuffer);

            // Render transparent objects
            if (m_settings.get_transparency_activated())
            {
                int m_transparency = m_settings.get_transparency_mode();
                switch (m_transparency)
                {
                    case DEPTH_PEELING:
                        passes.transparency_pass_dp->render(this);
                        break;
                    case WEIGHTED_BLENDED :
                        passes.transparency_pass_wb->render(this);
                    default:
                        return;
                }
            }

            // Render Selection
            if (m_settings.get_selection_activated())
            {
                m_selectionFrameBuffer->render(this);
            }
        }

        // set render states
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE); // enable depth writes so glClear won't ignore clearing the depth buffer
        glDisable(GL_BLEND);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // copy multisampled framebuffer that we rendered on to the imgui texture for display
        FrameBufferObject::copy(GL_COLOR_ATTACHMENT0, GL_COLOR_BUFFER_BIT, buffers.target_framebuffer_ms, buffers.target_framebuffer);
    }

    void Renderer::handle_input()
    {
        Input::update();
        ImVec2 vMin = ImGui::GetWindowContentRegionMin();
        ImVec2 vMax = ImGui::GetWindowContentRegionMax();
        vMin.x += ImGui::GetWindowPos().x;
        vMin.y += ImGui::GetWindowPos().y;
        vMax.x += ImGui::GetWindowPos().x;
        vMax.y += ImGui::GetWindowPos().y;

        // mouse movement
        auto mouse_coords = Input::get_mouse_coords();
        input.pos.x = mouse_coords.x;
        input.pos.y = mouse_coords.y;
        auto is_down = Input::mouse_pressed();

        input.offset.x = 0.0f;
        input.offset.y = 0.0f;

        if (input.pos.x > vMin.x && input.pos.x < vMax.x && input.pos.y > vMin.y && input.pos.y < vMax.y)
        {
            if (is_down)
            {
                input.offset.x = input.pos.x - input.last.x;
                input.offset.y = input.last.y - input.pos.y;
            }
        }

        handle_camera_input();
        handle_mesh_input();

        input.last.x = input.pos.x;
        input.last.y = input.pos.y;
    }

    void Renderer::handle_mesh_input()
    {
        if(input.mesh_moving)
        {
            input.mesh_moving = false;
        }

        if (ImGui::IsWindowFocused() && ImGui::IsWindowHovered())
        {
            if (auto mesh = mesh_list->get_focused_mesh())
            {
                if (Input::key_down(Input::TRANSLATE_MESH))
                {
                    input.mesh_moving = true;
                    auto delta_x = (float) (2 * M_PI / frame.width);
                    auto delta_y = (float) (M_PI / frame.height);

                    glm::vec3 vertical = camera->get_right() * input.offset.x * delta_x * 2.0f;
                    glm::vec3 horizontal = camera->get_world_up() * input.offset.y * delta_y * 2.0f;

                    mesh->translate(mesh->get_data().position + vertical + horizontal) ;
                }
                else if (Input::key_down(Input::ROTATE_MESH))
                {
                    input.mesh_moving = true;
                    auto delta_x = (float) (2 * M_PI / frame.width);
                    auto delta_y = (float) (M_PI / frame.height);

                    float x_rotation = input.offset.x * delta_x;
                    float y_rotation = input.offset.y * delta_y;

                    auto pos = mesh->get_data().position;
                    auto off = mesh->get_data().position_offset;

//                    auto x_axis = glm::inverse(mesh->get_data().get_transform()) * glm::vec4(cam.get_up(), 0.0f);
//                    auto y_axis = glm::inverse(mesh->get_data().get_transform()) * glm::vec4(cam.get_right(), 0.0f);
//
//                    mesh->rotate(x_rotation, x_axis);
//                    mesh->rotate(-y_rotation, y_axis);

                    if (Input::mouse_pressed() && (input.last.x != input.pos.x || input.last.y != input.pos.y))
                    {
                        auto axis = TrackBall::get_rotation_axis({input.last.x, input.last.y}, {input.pos.x, input.pos.y},
                                                                 camera->get_viewport_size());
                        auto angle = TrackBall::get_rotation_angle({input.last.x, input.last.y}, {input.pos.x, input.pos.y},
                                                                   camera->get_viewport_size());
                        //glm::mat3 camera_to_trackball = glm::inverse(mesh->get_data().get_transform()) * glm::mat3(cam.world));
                        glm::vec3 axis_in_trackball_coords = glm::inverse(camera->view * mesh->get_data().get_transform()) * glm::vec4(axis, 0.0f);


                        mesh->rotate(angle, axis_in_trackball_coords);
                    }
                }
            }
        }
    }

    void Renderer::handle_camera_input()
    {
        if (ImGui::IsWindowFocused() && ImGui::IsWindowHovered())
        {
            if (!input.mesh_moving)
            {
                if (camera->animation)
                {
                    camera->animation_step();
                    camera->update();
                    return;
                }

                if (Input::mouse_double_clicked())
                {
                    auto mesh_id = passes.selection_hover_pass->get_hovered_mesh_object();

                    if (mesh_id >= 0)
                    {
                        auto mesh = mesh_list->get_mesh(mesh_id);
                        glm::vec3 new_target = {0.0f, 0.0f, 0.0f};

                        auto mode = GlobalViewerSettings::getInstance()->get_selection_mode();
                        if (mode == Selection::ALL)
                        {
                            new_target = mesh->get_data().position;
                        }
                        else if(mode != Selection::Off)
                        {
                            auto transform = camera->world * mesh->get_data().get_transform();
                            auto pos_mesh_space = glm::vec4(passes.selection_hover_pass->hover_position, 1.0f);
                            //new_target = mesh->get_data().position_offset + glm::vec3(transform * pos_mesh_space);
                            new_target = glm::vec3(transform * pos_mesh_space);
                        }

                        if (camera->get_mode() == FLY)
                        {
                            camera->switch_mode(new_target);
                        }
                        else
                        {
                            camera->animated_look_at(new_target);
                        }

                        mesh_list->set_focused_mesh(mesh_id);
                    }
                }

                if (Input::key_pressed(Input::SWITCH_CAMERA_MODE))
                {
                    glm::vec3 new_target = camera->target;
                    if(auto mesh = mesh_list->get_focused_mesh())
                    {
                        new_target = mesh->get_data().position;
                    }
                    camera->switch_mode(new_target);
                }

                camera->handle_mouse_scroll(Input::get_scroll_offset());
                camera->handle_mouse_movement(input.offset.x, input.offset.y);
                camera->handle_key_movement(Input::get_wasd_movement_vector());
            }

        }
        camera->update();
    }


    void Renderer::render_selection()
    {
        // now render our mesh scene to the framebuffer texture
        buffers.selection_frame_buffer->bind();

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

        GLubyte* data = buffers.pixel_buffer->start_read(x, y, 1, 1);

        if (data != nullptr)
        {
            // evaluate ID out of color
            int type = data[0] & 3;
            int id;
            if (passes.selection_pass->is_debug_mode())
            {
                id = (data[0] + data[1] * 256 + data[2] * 256 * 256) >> 2;
            }
            else
            {
                id = (data[0] + data[1] * 256 + data[2] * 256 * 256 + data[3] * 256 * 256 * 256) >> 2;
            }
            query_selection(type, id);
        }

        buffers.pixel_buffer->finish_read();

        m_current_frame = (m_current_frame + 1) % m_frame_limit;
        if (m_current_frame == 0)
        {
            // we need to clear our framebuffer as well
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            mesh_list->iterate([&](auto id, auto mesh){
                if(mesh->get_data().visible)
                {
                    mesh->update_vertex_buffer();
                    if (mesh->get_vao() != nullptr)
                    {
                        passes.selection_pass->render_mesh(mesh);
                    }
                }
            });
        }
        buffers.selection_frame_buffer->unbind();

        buffers.target_framebuffer_ms->bind();

        mesh_list->iterate([&](auto id, auto mesh){
            if(mesh->get_data().visible)
            {
                mesh->update_vertex_buffer();
                if (mesh->get_vao() != nullptr)
                {
                    passes.selection_hover_pass->render(nullptr, mesh);
                }
            }
        });

        buffers.target_framebuffer_ms->unbind();
    }

    void Renderer::query_selection(int type, int id)
    {
        m_selection_callback(type, id);
    }

    void Renderer::render_pre_pass()
    {
        passes.pre_pass->get_framebuffer()->bind();
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        passes.pre_pass->clear_position_buffer();


        mesh_list->iterate([&](auto id, auto mesh){
            if(mesh->get_data().visible)
            {
                mesh->update_vertex_buffer();
                auto vao = mesh->get_vao();
                if (mesh->get_data().rounding_active)
                {
                    vao = mesh->get_mvb()->get_vao_rounded();
                }
                if (vao != nullptr)
                {
                    passes.pre_pass->render(vao, mesh);
                }
            }
        });

        // we generate a mipmap for the position, this is used for ssao
        // this needs to happen every frame, since the fragment position values always change
        glBindTexture(GL_TEXTURE_2D, passes.pre_pass->get_framebuffer()->get_position_texture());
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        passes.pre_pass->get_framebuffer()->unbind();
    }

    void Renderer::render_shadow_map()
    {
        // render opaque shadow map
        glClearColor(0.0, 0.0, 0.0, 0.0);

        // calculate all cascade matrices
        passes.shadow_pass->clear_cascades();
        int cascade_level = GlobalViewerSettings::getInstance()->get_cascade_level();

        passes.shadow_pass->calculate_cascades(camera->near, camera->far, cascade_level);

        for (int i = 0; i < cascade_level; i++)
        {
            passes.shadow_pass->get_framebuffer()->bind();
            passes.shadow_pass->set_cascade_index(i);
            passes.shadow_pass->bind_for_writing(i);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            passes.shadow_pass->render(vao, mesh);

            passes.shadow_pass->get_framebuffer()->unbind();
        }
    }

    void Renderer::render_ssao_pass()
    {
        passes.ssao_pass->render(nullptr, nullptr);
    }

    void Renderer::render_transparency_wb()
    {
        passes.transparency_pass_wb->bind_transparent_buffer();
        passes.transparency_pass_wb->clear_framebuffer();
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunci(0, GL_ONE, GL_ONE);
        glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
        glBlendEquation(GL_FUNC_ADD);
        //glDisable(GL_CULL_FACE);

        mesh_list->iterate([&](auto id, auto mesh){
            if(mesh->get_data().visible)
            {
                mesh->update_vertex_buffer();
                auto vao = mesh->get_vao();
                if (mesh->get_data().rounding_active)
                {
                    vao = mesh->get_mvb()->get_vao_rounded();
                }
                if (vao != nullptr)
                {
                    passes.transparency_pass_wb->render(vao, mesh);
                }
            }
        });

        passes.transparency_pass_wb->unbind_transparent_buffer();

        glDepthFunc(GL_ALWAYS);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);

        buffers.target_framebuffer_ms->bind();
        passes.transparency_pass_wb->render_composition();
        buffers.target_framebuffer_ms->unbind();
    }

    void Renderer::render_transparency_dp()
    {
        int num_passes = m_settings.get_number_passes();
        for (int i = 0; i < num_passes; i++)
        {
            if (i % 2 == 0)
            {
                passes.transparency_pass_dp->m_transparent_framebuffer0->bind();
            }
            else
            {
                passes.transparency_pass_dp->m_transparent_framebuffer1->bind();
            }
            glClearDepth(0.0f);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

            // first render all meshes
            mesh_list->iterate([&](auto id, auto mesh){
                if(mesh->get_data().visible)
                {
                    mesh->update_vertex_buffer();
                    auto vao = mesh->get_vao();
                    if (mesh->get_data().rounding_active)
                    {
                        vao = mesh->get_mvb()->get_vao_rounded();
                    }
                    if (vao != nullptr)
                    {
                        passes.transparency_pass_dp->render(vao, mesh);
                    }
                }
            });

            if (i % 2 == 0)
            {
                passes.transparency_pass_dp->m_transparent_framebuffer0->unbind();
            }
            else
            {
                passes.transparency_pass_dp->m_transparent_framebuffer1->unbind();
            }
            passes.transparency_pass_dp->render_composition(i, num_passes);
        }
    }


    void Renderer::render_background()
    {

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        buffers.target_framebuffer_ms->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        passes.background_pass->render(nullptr, nullptr);
        buffers.target_framebuffer_ms->unbind();
    }


    void Renderer::render_transparency()
    {
        int m_transparency = m_settings.get_transparency_mode();
        switch (m_transparency)
        {
            case DEPTH_PEELING:
                render_transparency_dp();
                break;
            case WEIGHTED_BLENDED :
                render_transparency_wb();
            default:
                return;
        }
    }

    void Renderer::set_target_framebuffer(std::shared_ptr<FrameBufferObject> target_ms, std::shared_ptr<FrameBufferObject> target)
    {
        buffers.target_framebuffer_ms = std::move(target_ms);
        buffers.target_framebuffer = std::move(target);
    }
}