
#include "Camera.h"
#include "input/Input.h"
#include "../../util/VecUtil.h"

namespace vOS
{

    /**
     * 1) Trackball
     *
     */

    Camera::Camera():
        m_trackball(*this, 1.0f)
    {

        set_viewport_size(800, 600);

        // Init Position etc
        position = glm::vec3{0.0f, 0.0f, 10.0f};
        m_camera_front = glm::vec3{0.0f, 0.0f, -1.0f};
        m_camera_up = glm::vec3{0.0f, 1.0f, 0.0f};

        // Axis
        m_world_up = glm::vec3(0.0f, 1.0f, 0.0f);

        // set up the initial camera position, direction and orientation of the mesh
        world = glm::mat4(1.0f);
        view = glm::lookAt(
                position,
                target,
                m_camera_up
        );

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

        auto euler_angles = glm::degrees(glm::eulerAngles(orientation));

        glm::vec3 front;
        front.x = (float)sin(glm::radians(euler_angles.y));
        front.y = -(float)(sin(glm::radians(euler_angles.x)) * cos(glm::radians(euler_angles.y)));
        front.z = -(float)(cos(glm::radians(euler_angles.x)) * cos(glm::radians(euler_angles.y)));

//        m_camera_front = glm::normalize(front);
//        m_camera_right = glm::normalize(glm::cross(m_camera_front, m_camera_up));
//        m_camera_up = glm::normalize(glm::cross(m_camera_right, m_camera_front));

        projection = glm::perspective(
                glm::radians(zoom),
                (float) m_screen_width / (float) m_screen_height,
                near,
                far
        );

        // Calculate View Matrix with camera front vector offset as target
        view = glm::lookAt(
                position,
                position + m_camera_front,
                m_camera_up
        );

        orientation = glm::quat(glm::radians(euler_angles));

    }

    void Camera::handle_input()
    {
        // If left ctrl key is pressed, the object move mode is active which requires the camera stand still
        bool ignore_input = Input::controll_pressed();
        if(ignore_input)
            return;

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

                handle_trackball_movement({last_x, last_y}, {xpos, ypos});

                last_x = xpos;
                last_y = ypos;

                //handle_mouse_movement(x_offset, y_offset);
            } else
            {
                last_x = xpos;
                last_y = ypos;
            }
        }

        // 3 Dimensional Movement
//        if (ImGui::IsWindowFocused())
//        {
//
//            // X for Horizontal Movement
//            // Y for Vertical Movement
//            // Z for Forward Movement
//            glm::vec3 input_vector = {Input::get_wasd_movement_vector_X() * m_horizontal_speed,
//                                      Input::get_wasd_movement_vector_Y() * m_vertical_speed,
//                                      Input::get_wasd_movement_vector_Z() * m_vertical_speed};
//            input_vector *= delta;
//            if (m_mode == FLY)
//            {
//                // Add the movement vector to the position
//                glm::vec3 mov_vector = input_vector.x * m_camera_right +
//                                       input_vector.y * m_camera_up +
//                                       input_vector.z * m_camera_front;
//
//                position += mov_vector;
//            }
//            else if (m_mode == ORBIT)
//            {
//
//            }
//        }
    }

    void Camera::handle_trackball_movement(const glm::vec2 start_position, const glm::vec2& end_position)
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
            zoom -= m_zoom_strength * (float) y_offset;
            if (zoom < 1.0f)
            {
                zoom = 1.0f;
            }
            if (zoom > 90.0f)
            {
                zoom = 90.0f;
            }
        }
        if (m_mode == ORBIT)
        {

        }
    }

    void Camera::handle_mouse_movement(float x_offset, float y_offset)
    {
        if (m_mode == FLY)
        {
            x_offset *= m_sensitivity;
            y_offset *= m_sensitivity;

            //rotate around x_axis
            orientation = glm::normalize(glm::rotate(orientation, glm::radians(1.0f * x_offset), m_camera_up));
            //rotate around y_axis
            orientation = glm::normalize(glm::rotate(orientation, -glm::radians(1.0f * y_offset), m_camera_right));

            //TODO: Make sure vertical angle does not get higher than 89° or lower than -89°

            auto euler_angles = glm::degrees(glm::eulerAngles(orientation));
            std::cout << euler_angles.y << std::endl;
        }
        else if (m_mode == ORBIT)
        {
            x_offset *= m_sensitivity;
            y_offset *= m_sensitivity;

        }

    }

    void Camera::set_mode(Mode mode)
    {
        m_mode = mode;
    }

    void Camera::look_at(glm::vec3 target)
    {
        return;
    }

    glm::vec2 Camera::get_viewport_size() const
    {
        return glm::vec2{m_screen_width, m_screen_height};
    }

    const glm::vec3& Camera::get_front() const
    {
        return m_camera_front;
    }

    const glm::vec3& Camera::get_up() const
    {
        return m_camera_up;
    }

    const glm::vec3& Camera::get_right() const
    {
        return m_camera_right;
    }
}