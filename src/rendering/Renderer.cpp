
#include "Renderer.h"

#include "volumeshOS.h"
#include "input/Input.h"
#include "../util/StringUtil.h"

#include <stb_image_write.h>

namespace volumeshOS::Internal
{

    Renderer::Renderer(
            int width,
            int height
    )
    {
        frame.width = width;
        frame.height = height;

        if(AppState::settings.multisampling)
        {
            buffers.target_framebuffer_ms = std::make_shared<FrameBufferObject>(width, height,
                                                                                FrameBufferObject::RGBA_AND_DEPTH_MULTISAMPLE);
        }
        else
        {
            buffers.target_framebuffer_ms = std::make_shared<FrameBufferObject>(width, height,
                                                                                FrameBufferObject::RGBA_AND_DEPTH);
        }

        buffers.target_framebuffer = std::make_shared<FrameBufferObject>(width, height,
                                                                         FrameBufferObject::RGBA_AND_DEPTH);
        buffers.post_framebuffer = std::make_shared<FrameBufferObject>(width, height,
                                                                       FrameBufferObject::RGBA_AND_DEPTH);
        buffers.selection_frame_buffer = std::make_shared<FrameBufferObject>(width / 2, height / 2,
                                                                             FrameBufferObject::RGBA_AND_DEPTH);
        buffers.pixel_buffer = std::make_shared<PixelBufferObject>(2, width / 2, height / 2);

        passes.background_pass = std::make_shared<BackgroundPass>();
        passes.ground_pass = std::make_shared<GroundPass>();
        passes.pre_pass = std::make_shared<PrePass>(width, height);
        passes.shadow_pass = std::make_shared<ShadowMapPass>(width, height);
        passes.mesh_pass = std::make_shared<MeshPass>();
        passes.ssao_pass = std::make_shared<SSAOPass>(width, height);
        passes.transparency_pass_wb = std::make_shared<TransparencyPassWB>(*this, width, height);
        passes.transparency_pass_dp = std::make_shared<TransparencyPassDP>(width, height);
        //passes.shape_pass               = std::make_shared<ShapePass>();
        passes.selection_pass = std::make_shared<SelectionPass>();
        passes.selection_hover_pass = std::make_shared<SelectionHoverPass>();
        passes.vertex_only_pass = std::make_shared<VertexOnlyPass>();
        passes.post_processing_pass = std::make_shared<PostProcessingPass>();
        passes.outline_pass = std::make_shared<OutlinePass>();

        input.last.x = (float) width / 2.0f;
        input.last.y = (float) height / 2.0f;

        mesh_list = std::make_shared<MeshList>();
        camera = std::make_shared<Camera>();
        camera->set_viewport_size((float) width, (float) height);
        shapes = std::make_shared<ShapeRenderer>();

    }

    void Renderer::resize(int width, int height)
    {
        frame.width = width;
        frame.height = height;
        buffers.target_framebuffer_ms->resize(width, height);
        buffers.target_framebuffer->resize(width, height);
        buffers.post_framebuffer->resize(width, height);
        passes.transparency_pass_wb->resize_buffers(*this, width, height);
        passes.transparency_pass_dp->resize_buffers(width, height);
        passes.pre_pass->resize_buffers(width, height);
        passes.ssao_pass->resize_buffers(width, height);
        passes.shadow_pass->resize_buffers(width, height);
        buffers.selection_frame_buffer->resize(width / 2, height / 2);
        buffers.pixel_buffer = std::make_shared<PixelBufferObject>(2, width / 2, height / 2);
        input.last.x = (float) width / 2.0f;
        input.last.y = (float) height / 2.0f;
        camera->set_viewport_size((float) width, (float) height);
        camera->update();
    }

    bool Renderer::should_render_mesh(const std::shared_ptr<MeshObject>& mesh)
    {
        return mesh->get_data().visible && mesh->get_vao() != nullptr;
    }

    void Renderer::update_pass_data()
    {
        pass_data_list.clear();
        PassData pass_data = {};

        auto light = AppState::settings.light;
        auto cam = camera;

        for(const auto& mesh : render_list)
        {
            pass_data.data = mesh->get_data();
            pass_data.transform = cam->world * pass_data.data.get_transform();
            pass_data.projection = cam->projection;
            pass_data.view = cam->view;
            pass_data.view_transform = cam->view * pass_data.transform;
            pass_data.view_dir = glm::normalize(cam->target - cam->position);
            pass_data.slice_direction = mesh->get_slice_dir(pass_data.transform, -glm::normalize(cam->get_front()));
            pass_data.cam_pos = cam->position;
            pass_data.light_pos = glm::normalize(light.direction);
            pass_data.light_color = light.color;
            pass_data.light_transform = pass_data.transform;

            auto bb = mesh->get_world_bb(pass_data.view_transform);
            pass_data.bb_min = bb.first;
            pass_data.bb_max = bb.second;

            pass_data_list[mesh->get_id()] = pass_data;
        }
    }

    void Renderer::render(const RenderData& data)
    {
        // reset statistics for the new frame
        AppState::statistics = {};

        auto& settings = AppState::settings;

        frame.current = (frame.current + 1) % frame.limit;
        frame.is_rendering_background = data.render_bg;
        frame.ground_shadow_only = data.ground_shadow_only;

        // handle input
        if (data.update_input)
        {
            handle_input();
        }

        std::cout << "Width " << frame.width << std::endl;
        std::cout << "Height " << frame.height << std::endl;

        render_list.clear();
        mesh_list->iterate([&](auto id, auto mesh)
                           {
                               mesh->update_vertex_buffer();
                               if (should_render_mesh(mesh))
                               {
                                   render_list.push_back(mesh);
                               }
                           });
        update_pass_data();

        buffers.target_framebuffer_ms->bind();
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glStencilMask(0xFF);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        buffers.target_framebuffer_ms->unbind();

        // Render Meshes
        passes.pre_pass->render(*this);

        // Checks for double click to orbit camera around clicked point and zooms in
        handle_zoom();

        if (settings.ssao_active)
        {
            passes.ssao_pass->render(*this);
        }

        if (settings.shadows_active)
        {
            passes.shadow_pass->render(*this);
        }

        if (data.render_bg)
        {
            passes.background_pass->render(*this);
        }

        if (data.render_ground)
        {
            passes.ground_pass->render(*this);
        }
        passes.vertex_only_pass->render(*this);
        passes.mesh_pass->render(*this);

        // render shapes
        if (settings.shapes_active && data.render_shapes)
        {
            shapes->render(*this);
        }

        FrameBufferObject::copy(
                GL_DEPTH_STENCIL_ATTACHMENT,
                GL_DEPTH_ATTACHMENT,
                GL_DEPTH_BUFFER_BIT,
                buffers.target_framebuffer_ms,
                buffers.target_framebuffer
        );


        // Render transparent objects
        if (settings.transparency_active)
        {
            switch (settings.transparency_mode)
            {
                case TransparencyMode::DEPTH_PEELING:
                    passes.transparency_pass_dp->render(*this);
                    break;
                case TransparencyMode::WEIGHTED_BLENDED:
                    passes.transparency_pass_wb->render(*this);
                    break;
                default:
                    return;
            }
        }

        // Render SelectionMode
        if (settings.selection_active)
        {
            passes.selection_pass->render(*this);
            passes.selection_hover_pass->render(*this);
        }


        //passes.outline_pass->render(*this);

        // set render states
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE); // enable depth writes so glClear won't ignore clearing the depth buffer
        glDisable(GL_BLEND);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // copy multisampled framebuffer that we rendered on to the imgui texture for display
        FrameBufferObject::copy(GL_COLOR_ATTACHMENT0, GL_COLOR_BUFFER_BIT, buffers.target_framebuffer_ms, buffers.target_framebuffer);
        FrameBufferObject::copy(GL_DEPTH_STENCIL_ATTACHMENT, GL_DEPTH_ATTACHMENT, GL_DEPTH_BUFFER_BIT, buffers.target_framebuffer_ms, buffers.target_framebuffer);

        passes.post_processing_pass->render(*this);
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

        if (!AppState::settings.block_input)
        {
            handle_camera_input();
            handle_mesh_input();
        }
        else
        {
            camera->update();
        }


        input.last.x = input.pos.x;
        input.last.y = input.pos.y;
    }

    void Renderer::handle_mesh_input()
    {
        if (input.mesh_moving)
        {
            input.mesh_moving = false;
        }

        if (ImGui::IsWindowHovered() && !input_blocking)
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

                    mesh->translate(mesh->get_data().position + vertical + horizontal);
                }
                else if (Input::key_down(Input::ROTATE_MESH))
                {
                    input.mesh_moving = true;
                    auto delta_x = (float) (2 * M_PI / frame.width);
                    auto delta_y = (float) (M_PI / frame.height);

                    float x_rotation = input.offset.x * delta_x;
                    float y_rotation = input.offset.y * delta_y;

                    auto pos = mesh->get_data().position;
                    auto off = mesh->get_data().origin;

//                    auto x_axis = glm::inverse(mesh->get_data().get_transform()) * glm::vec4(cam.get_up(), 0.0f);
//                    auto y_axis = glm::inverse(mesh->get_data().get_transform()) * glm::vec4(cam.get_right(), 0.0f);
//
//                    mesh->rotate(x_rotation, x_axis);
//                    mesh->rotate(-y_rotation, y_axis);

                    if (Input::mouse_pressed() && (input.last.x != input.pos.x || input.last.y != input.pos.y))
                    {
                        auto axis = TrackBall::get_rotation_axis({input.last.x, input.last.y},
                                                                 {input.pos.x, input.pos.y},
                                                                 camera->get_viewport_size());
                        auto angle = TrackBall::get_rotation_angle({input.last.x, input.last.y},
                                                                   {input.pos.x, input.pos.y},
                                                                   camera->get_viewport_size());
                        //glm::mat3 camera_to_trackball = glm::inverse(mesh->get_data().get_transform()) * glm::mat3(cam.world));
                        glm::vec3 axis_in_trackball_coords =
                                glm::inverse(camera->view * mesh->get_data().get_transform()) * glm::vec4(axis, 0.0f);


                        mesh->rotate(angle, axis_in_trackball_coords);
                    }
                }
                else if(Input::key_down(Input::FOCUS_CAMERA_ON_MESH))
                {
                    camera->animated_look_at(mesh->get_data().position);
                }
                auto undo_char = glfwGetKeyName(GLFW_KEY_Z, 0);
                auto undo_key = GLFW_KEY_Y;
                if (*undo_char == 'Z')
                    undo_key = GLFW_KEY_Z;

                if(ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) && ImGui::IsKeyPressed(undo_key))
                {
                    mesh->get_mvb()->undo_digging();
                }
                else if(ImGui::IsKeyDown(GLFW_KEY_LEFT_SHIFT) && ImGui::IsKeyPressed(undo_key))
                {
                    mesh->get_mvb()->undo_digging(10);
                }

            }
        }
    }

    void Renderer::handle_camera_input()
    {

        if (!input.mesh_moving && !input_blocking)
        {
            if (camera->is_animating)
            {
                camera->update();
                return;
            }
            if (ImGui::IsWindowHovered())
            {
                camera->handle_mouse_scroll(Input::get_scroll_offset());
                camera->handle_mouse_movement(input.offset.x, input.offset.y);
                camera->handle_key_movement(Input::get_wasd_movement_vector());

            }

            if (ImGui::IsWindowFocused())
            {
                if (Input::key_pressed(Input::SWITCH_CAMERA_MODE))
                {
                    glm::vec3 new_target = camera->target;
                    if (auto mesh = mesh_list->get_focused_mesh())
                    {
                        new_target = volumeshOS::get_focused_mesh().get_position<glm::vec3>();
                    }

                    if (camera->get_mode() == CameraMode::ORBIT)
                    {
                        camera->set_mode(CameraMode::FLY);
                    }
                    else if (camera->get_mode() == CameraMode::FLY)
                    {
                        camera->animated_look_at(new_target);
                        camera->set_mode(CameraMode::ORBIT);
                    }

                }
            }
        }
        camera->update();
    }

    void Renderer::export_image(const std::string& path, const ExportOptions& options)
    {
        int prev_width = frame.width;
        int prev_height = frame.height;

        int export_width = options.width > 0 ? options.width : frame.width;
        int export_height = options.height > 0 ? options.height : frame.height;

        // we need to do this since some passes need the current width and height for rendering
        frame.width = export_width;
        frame.height = export_height;

        std::cout << "Width " << export_width << std::endl;
        std::cout << "Height " << export_height << std::endl;


        auto export_framebuffer_ms = std::make_shared<FrameBufferObject>(export_width, export_height,
                                                                         FrameBufferObject::RGBA_AND_DEPTH_MULTISAMPLE);
        auto export_framebuffer = std::make_shared<FrameBufferObject>(export_width, export_height,
                                                                      FrameBufferObject::RGBA_AND_DEPTH);
        auto export_post_framebuffer = std::make_shared<FrameBufferObject>(export_width, export_height,
                                                                           FrameBufferObject::RGBA_AND_DEPTH);

        auto prev_target_framebuffer_ms = buffers.target_framebuffer_ms;
        auto prev_target_framebuffer = buffers.target_framebuffer;
        auto prev_post_framebuffer = buffers.post_framebuffer;

        buffers.target_framebuffer_ms = export_framebuffer_ms;
        buffers.target_framebuffer = export_framebuffer;
        buffers.post_framebuffer = export_post_framebuffer;

        resize(export_width, export_height);

        RenderData data;
        data.render_bg = options.include_background;
        data.render_shapes = options.include_shapes;
        data.render_ground = options.include_ground;
        data.ground_shadow_only = options.ground_shadow_only;
        data.update_input = false;
        render(data);

        glFlush();
        glFinish();

        export_post_framebuffer->bind();

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        int sWidth = export_framebuffer->get_width();
        int sHeight = export_framebuffer->get_height();
        std::vector<unsigned char> buffer(4 * sWidth * sHeight);

        glReadPixels(0, 0, sWidth, sHeight, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());

        stbi_flip_vertically_on_write(true);

        auto split = StringUtil::split_str(path, ".");
        std::string extension = split[split.size() - 1];

        if (extension == "bmp")
        {
            stbi_write_bmp(path.c_str(), sWidth, sHeight, 4, buffer.data());
        }
        else if (extension == "png")
        {
            stbi_write_png(path.c_str(), sWidth, sHeight, 4, buffer.data(), 4 * sWidth);
        }

        export_post_framebuffer->unbind();

        // restore the old width and height
        frame.width = prev_width;
        frame.height = prev_height;
        buffers.target_framebuffer_ms = prev_target_framebuffer_ms;
        buffers.target_framebuffer = prev_target_framebuffer;
        buffers.post_framebuffer = prev_post_framebuffer;
        resize(frame.width, frame.height);
    }

    void Renderer::handle_zoom()
    {
        if (Input::mouse_double_clicked())
        {
            if (ImGui::IsWindowHovered())
            {
                // now render our mesh scene to the framebuffer texture
                //passes.pre_pass->get_framebuffer()->bind();
                buffers.target_framebuffer->bind();

                // viewport (0,0) starts top left, but framebuffer (0,0) starts bottom left
                // viewport[3] equals viewport height
                GLint viewport[4];
                glGetIntegerv(GL_VIEWPORT, viewport);

                ImVec2 mouse_pos_in_window = {
                        ImGui::GetMousePos().x - ImGui::GetCursorScreenPos().x - ImGui::GetScrollX(),
                        ImGui::GetMousePos().y - ImGui::GetCursorScreenPos().y - ImGui::GetScrollY()
                };
                int x = (int) mouse_pos_in_window.x;
                int y = (int) (viewport[3] - (int) mouse_pos_in_window.y);

                GLfloat depth = 0.0;

                glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
                if (depth >= 0.99999)
                    return;

                // get NDCs
                glm::vec4 pos;
                pos.x = 2.0f * ((float) x / (float) viewport[2]) - 1.0f;
                pos.y = 2.0f * ((float) y / (float) viewport[3]) - 1.0f;
                pos.z = 2.0f * depth - 1.0f;
                pos.w = 1.0;

                // unproject from clip space to world space
                pos = glm::inverse(camera->projection) * pos;
                pos /= pos.w;
                pos = glm::inverse(camera->view) * pos;

                glm::vec3 new_cam_pos = camera->position + 0.5f * (glm::vec3(pos) - camera->position);

                if (camera->get_mode() != CameraMode::ORBIT)
                    camera->set_mode(CameraMode::ORBIT);
                camera->animated_look_at(glm::vec3(pos), new_cam_pos);


                buffers.target_framebuffer->unbind();

            }
        }
    }
}