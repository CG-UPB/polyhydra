
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

        buffers.target_framebuffer_ms = std::make_shared<FrameBufferObject>(width, height, FrameBufferObject::RGBA_AND_DEPTH_MULTISAMPLE);
        buffers.target_framebuffer = std::make_shared<FrameBufferObject>(width, height, FrameBufferObject::RGBA_AND_DEPTH);
        buffers.selection_frame_buffer = std::make_shared<FrameBufferObject>(width / 2, height / 2,
                                                                             FrameBufferObject::RGBA_AND_DEPTH);
        buffers.pixel_buffer = std::make_shared<PixelBufferObject>(2, width / 2, height / 2);

        passes.background_pass = std::make_shared<BackgroundPass>();
        passes.ground_pass = std::make_shared<GroundPass>();
        passes.pre_pass = std::make_shared<PrePass>(width, height);
        passes.shadow_pass = std::make_shared<ShadowMapPass>(width * 2, height * 2);
        passes.mesh_pass = std::make_shared<MeshPass>();
        passes.ssao_pass = std::make_shared<SSAOPass>(width, height);
        passes.transparency_pass_wb = std::make_shared<TransparencyPassWB>(*this, width, height);
        passes.transparency_pass_dp = std::make_shared<TransparencyPassDP>(width, height);
        //passes.shape_pass               = std::make_shared<ShapePass>();
        passes.selection_pass = std::make_shared<SelectionPass>();
        passes.selection_hover_pass = std::make_shared<SelectionHoverPass>();
        passes.vertex_only_pass = std::make_shared<VertexOnlyPass>();

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
        buffers.target_framebuffer_ms->resize(frame.width, frame.height);
        buffers.target_framebuffer->resize(frame.width, frame.height);
        passes.transparency_pass_wb->resize_buffers(*this, frame.width, frame.height);
        passes.transparency_pass_dp->resize_buffers(frame.width, frame.height);
        passes.pre_pass->resize_buffers(frame.width, frame.height);
        passes.ssao_pass->resize_buffers(frame.width, frame.height);
        passes.shadow_pass->resize_buffers(frame.width * 2, frame.height * 2);
        buffers.selection_frame_buffer->resize(frame.width / 2, frame.height / 2);
        buffers.pixel_buffer = std::make_shared<PixelBufferObject>(2, frame.width / 2, frame.height / 2);
        input.last.x = (float) width / 2.0f;
        input.last.y = (float) height / 2.0f;
        camera->set_viewport_size((float) width, (float) height);
    }

    bool Renderer::should_render_mesh(const std::shared_ptr<MeshObject>& mesh)
    {
        return mesh->get_data().visible && mesh->get_vao() != nullptr;
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

        render_list.clear();
        mesh_list->iterate([&](auto id, auto mesh){
            mesh->update_vertex_buffer();
            if (should_render_mesh(mesh))
            {
                render_list.push_back(mesh);
            }
        });

        buffers.target_framebuffer_ms->bind();
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        buffers.target_framebuffer_ms->unbind();

        // Render Meshes
        passes.pre_pass->render(*this);

        if (settings.rendering_mode == RenderingMode::ONLY_VERTICES)
        {
            if (data.render_bg)
            {
                passes.background_pass->render(*this);
            }
            passes.vertex_only_pass->render(*this);
            if (data.render_ground)
            {
                passes.ground_pass->render(*this);
            }
        }
        else
        {
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

            passes.mesh_pass->render(*this);

            FrameBufferObject::copy(GL_DEPTH_ATTACHMENT, GL_DEPTH_BUFFER_BIT, buffers.target_framebuffer_ms,
                                    buffers.target_framebuffer);

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
        }

        // render shapes
        if (settings.shapes_active && data.render_shapes)
        {
            shapes->render(*this);
        }

        // set render states
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE); // enable depth writes so glClear won't ignore clearing the depth buffer
        glDisable(GL_BLEND);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // copy multisampled framebuffer that we rendered on to the imgui texture for display
        FrameBufferObject::copy(GL_COLOR_ATTACHMENT0, GL_COLOR_BUFFER_BIT, buffers.target_framebuffer_ms,
                                buffers.target_framebuffer);
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
                    auto off = mesh->get_data().position_offset;

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
                if (Input::mouse_double_clicked())
                {
                    auto mesh_id = passes.selection_hover_pass->get_hovered_mesh_object();

                    if (mesh_id >= 0)
                    {
                        auto mesh = mesh_list->get_mesh(mesh_id);
                        glm::vec3 new_target = mesh->get_data().position;

                        auto mode = AppState::settings.selection_mode;
                        if (mode != SelectionMode::OFF)
                        {
                            auto transform = camera->world * mesh->get_data().get_transform();
                            auto pos_mesh_space = glm::vec4(hover_position, 1.0f);
                            //new_target = mesh->get_data().position_offset + glm::vec3(transform * pos_mesh_space);
                            new_target = glm::vec3(transform * pos_mesh_space);
                        }
                        camera->animated_look_at(new_target);

                        mesh_list->set_focused_mesh(mesh_id);
                    }
                }
                camera->handle_mouse_scroll(Input::get_scroll_offset());
                camera->handle_mouse_movement(input.offset.x, input.offset.y);
                camera->handle_key_movement(Input::get_wasd_movement_vector());

            }

            if(ImGui::IsWindowFocused())
            {
                if (Input::key_pressed(Input::SWITCH_CAMERA_MODE))
                {
                    glm::vec3 new_target = camera->target;
                    if (auto mesh = mesh_list->get_focused_mesh())
                    {
                        new_target = mesh->get_data().position;
                    }

                    if(camera->get_mode() == CameraMode::ORBIT)
                    {
                        camera->set_mode(CameraMode::FLY);
                    }
                    else if(camera->get_mode() == CameraMode::FLY)
                    {
                        camera->set_target(new_target);
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

        auto export_framebuffer_ms = std::make_shared<FrameBufferObject>(export_width, export_height,FrameBufferObject::RGBA_AND_DEPTH_MULTISAMPLE);
        auto export_framebuffer = std::make_shared<FrameBufferObject>(export_width, export_height, FrameBufferObject::RGBA_AND_DEPTH);

        auto prev_target_framebuffer_ms = buffers.target_framebuffer_ms;
        auto prev_target_framebuffer = buffers.target_framebuffer;

        buffers.target_framebuffer_ms = export_framebuffer_ms;
        buffers.target_framebuffer = export_framebuffer;

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

        export_framebuffer_ms->unbind();
        export_framebuffer->bind();

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        int sWidth = export_framebuffer->get_width();
        int sHeight = export_framebuffer->get_height();
        std::vector<unsigned char> buffer(4 * sWidth * sHeight);

        glReadPixels(0, 0, sWidth, sHeight, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());

        // since we are rendering on a transparent background, we need to undo the pre-multiplication
        for (size_t i = 0; i < buffer.size() / 4; i++)
        {
            float r = (float) ((int) buffer[i * 4]) / 255.0f;
            float g = (float) ((int) buffer[i * 4 + 1]) / 255.0f;
            float b = (float) ((int) buffer[i * 4 + 2]) / 255.0f;
            float a = (float) ((int) buffer[i * 4 + 3]) / 255.0f;
            float alpha = std::max(a, 0.00001f);
            buffer[i * 4] = (unsigned char) ((int) ((r / alpha) * 255.0f));
            buffer[i * 4 + 1] = (unsigned char) ((int) ((g / alpha) * 255.0f));
            buffer[i * 4 + 2] = (unsigned char) ((int) ((b / alpha) * 255.0f));
        }

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

        export_framebuffer->unbind();

        // restore the old width and height
        frame.width = prev_width;
        frame.height = prev_height;
        buffers.target_framebuffer_ms = prev_target_framebuffer_ms;
        buffers.target_framebuffer = prev_target_framebuffer;
        resize(frame.width, frame.height);
    }
}