
#include "Camera.h"
#include "input/Input.h"
#include "../../util/VecUtil.h"

namespace vOS {

    /**
     * 1) Trackball
     *
     */

    Camera::Camera() :
            m_trackball(*this, 1.0f)
    {

        set_viewport_size(800, 600);

        // Init Position etc
        position = glm::vec3{0.0f, 0.0f, 20.0f};
        target = glm::vec3{0.0f, 0.0f, 0.0f};
        m_camera_front = glm::vec3{0.0f, 0.0f, -1.0f};
        m_camera_up = glm::vec3{0.0f, 1.0f, 0.0f};

        // Axis
        m_world_up = glm::vec3(0.0f, 1.0f, 0.0f);

        // set up the initial camera position, direction and orientation of the mesh
        world = glm::mat4(1.0f);
        view = glm::lookAt(
                position,
                target,
                m_world_up
        );
        m_sensitivity = 1.0f;
        m_vertical_speed = 2.0f;
        m_horizontal_speed = 1.0f;

        //set_mode(FLY);
        set_mode(ORBIT);
    }

    void Camera::set_viewport_size(float width, float height)
    {
        m_screen_width = width;
        m_screen_height = height;
        last_x = m_screen_width / 2.0f;
        last_y = m_screen_height / 2.0f;

    }

    void Camera::update()
    {
        Input::update();
        handle_input();

        // Frame Delta
        auto current_frame = (float) ImGui::GetTime();
        delta = current_frame - last_frame;
        last_frame = current_frame;

        projection = glm::perspective(
                glm::radians(zoom),
                m_screen_width / m_screen_height,
                near,
                far
        );

        view = glm::lookAt(
                position,
                target,
                m_world_up
        );
    }

    void Camera::handle_input()
    {
        // If left ctrl key is pressed, the object move mode is active which requires the camera stand still
        bool ignore_input = Input::controll_pressed();
        if (ignore_input)
            return;

        if (Input::camera_mode_switch_pressed())
        {
            if (m_mode == FLY)
            {
                m_mode = ORBIT;
                target = glm::vec3(0.0f, 0.0f, 0.0f);
            }
            else
            {
                m_mode = FLY;
            }
        }

        ImVec2 vMin = ImGui::GetWindowContentRegionMin();
        ImVec2 vMax = ImGui::GetWindowContentRegionMax();
        vMin.x += ImGui::GetWindowPos().x;
        vMin.y += ImGui::GetWindowPos().y;
        vMax.x += ImGui::GetWindowPos().x;
        vMax.y += ImGui::GetWindowPos().y;

        // mouse movement
        auto xpos = (float) Input::get_mouse_X();
        auto ypos = (float) Input::get_mouse_Y();
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
        handle_mouse_scroll((float) Input::get_scroll_offset_Y());

        if (xpos > vMin.x && xpos < vMax.x && ypos > vMin.y && ypos < vMax.y)
        {
            if (is_down)
            {
                if (first_mouse)
                {
                    last_x = xpos;
                    last_y = ypos;
                    first_mouse = false;
                }

                float x_offset = xpos - last_x;
                float y_offset = last_y - ypos;

                //handle_trackball_movement({last_x, last_y}, {xpos, ypos});

                last_x = xpos;
                last_y = ypos;

                handle_mouse_movement(x_offset, y_offset);
            } else
            {
                last_x = xpos;
                last_y = ypos;
            }
        }

        // 3 Dimensional Movement
        if (ImGui::IsWindowFocused())
        {

            // X for Horizontal Movement
            // Y for Vertical Movement
            // Z for Forward Movement
            glm::vec3 input_vector = {Input::get_wasd_movement_vector_X(),
                                      Input::get_wasd_movement_vector_Y(),
                                      Input::get_wasd_movement_vector_Z()};
            input_vector *= delta * 10;
            if (m_mode == FLY)
            {
                // Add the movement vector to the position
                glm::vec3 mov_vector = input_vector.x * glm::normalize(get_right()) +
                                       input_vector.y * glm::normalize(m_world_up) +
                                       input_vector.z * glm::normalize(target - position);

                glm::vec3 view_dir = glm::normalize(target - position);
                position += mov_vector;
                target = position + view_dir;

            } else if (m_mode == ORBIT)
            {

            }
        }
    }

    void Camera::handle_trackball_movement(const glm::vec2 &start_position, const glm::vec2 &end_position)
    {
        //Log::warn("start x: " + std::to_string(start_position.x) + ", start y: " + std::to_string(start_position.y));
        //Log::warn("end x: " + std::to_string(end_position.x) + ", end y: " + std::to_string(end_position.y));
        if (start_position.x != end_position.x || start_position.y != end_position.y)
        {
            m_trackball.move_from_to(start_position, end_position);
            VecUtil::print_mat(view);
        }
    }

    void Camera::handle_mouse_scroll(float y_offset)
    {
        if (m_mode == FLY)
        {
            zoom -= m_zoom_strength * y_offset;
            if (zoom < 1.0f)
            {
                zoom = 1.0f;
            }
            if (zoom > 90.0f)
            {
                zoom = 90.0f;
            }
        } else if (m_mode == ORBIT)
        {
            position = position - y_offset * glm::normalize(position - target);
        }
    }

    void Camera::handle_mouse_movement(float x_offset, float y_offset)
    {
        x_offset *= m_horizontal_speed;
        y_offset *= m_vertical_speed;

        glm::vec4 pos(position.x, position.y, position.z, 1.0f);
        glm::vec4 tgt(target.x, target.y, target.z, 1.0f);

        auto delta_angle_x = (float) (2 * M_PI / m_screen_width);
        auto delta_angle_y = (float) (M_PI / m_screen_height);

        float angle_x = -x_offset * delta_angle_x;
        float angle_y = y_offset * delta_angle_y;

        // when front and up are simalar
        if (float cos_angle = dot(get_front(), m_world_up); cos_angle * glm::sign(angle_y) > 0.99f)
        {
            angle_y = 0.0f;
        }

        if (m_mode == FLY)
        {
            glm::mat4 rotation_mat_x(1.0f);
            rotation_mat_x = glm::rotate(rotation_mat_x, angle_x, m_world_up);
            tgt = (rotation_mat_x * (pos - tgt)) + pos;

            glm::mat4 rotation_mat_y(1.0f);
            rotation_mat_y = glm::rotate(rotation_mat_y, angle_y, get_right());
            target = (rotation_mat_y * (pos - tgt)) + pos;

        } else if (m_mode == ORBIT)
        {
            glm::mat4 rotation_mat_x(1.0f);
            rotation_mat_x = glm::rotate(rotation_mat_x, angle_x, m_world_up);
            pos = (rotation_mat_x * (pos - tgt)) + tgt;

            glm::mat4 rotation_mat_y(1.0f);
            rotation_mat_y = glm::rotate(rotation_mat_y, angle_y, get_right());
            position = (rotation_mat_y * (pos - tgt)) + tgt;
        }

    }

    void Camera::set_mode(Mode mode)
    {
        m_mode = mode;
    }

    void Camera::look_at(glm::vec3 new_target)
    {
        target = new_target;
        view = glm::lookAt(
                position,
                target,
                m_camera_up
        );
    }

    glm::vec2 Camera::get_viewport_size() const
    {
        return glm::vec2{m_screen_width, m_screen_height};
    }

    glm::vec3 Camera::get_front() const
    {
        return -glm::transpose(view)[2];
    }

    glm::vec3 Camera::get_up() const
    {
        return m_camera_up;
    }

    glm::vec3 Camera::get_right() const
    {
        return glm::transpose(view)[0];
    }

}