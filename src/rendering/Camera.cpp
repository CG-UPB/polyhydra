//
// Created by projektgruppe on 06.03.22.
//

#include "Camera.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "../input/Input.h"
#include "../util/VecUtil.h"
#include <iostream>

namespace vOS
{

    Camera::Camera()
    {

        set_viewport_size(800, 600);

        // Init Position etc
        position = glm::vec3{0.0f, 0.0f, 10.0f};
        m_camera_front = glm::vec3 {0.0f, 0.0f, -1.0f};
        m_camera_up =glm::vec3 {0.0f, 1.0f, 0.0f};

        // Axis
        m_world_up = glm::vec3(0.0f, 1.0f, 0.0f);

        // set up the initial camera position, direction and orientation of the mesh
        world = glm::mat4(1.0f);

        // setup for orbit mode
        radius = 10.0f;
        theta = 0.0f;
        phi = 90.0f;

        m_pitch = 0;
        m_yaw = 0;


        set_mode(ORBIT);


    }

    void Camera::set_viewport_size(float width, float height)
    {
        m_screen_width = width;
        m_screen_height = height;
        last_x = m_screen_width / 2.0f;
        last_y = m_screen_height / 2.0f;
    }

    void Camera::update() {
        // Frame Delta
        auto current_frame = (float) ImGui::GetTime();
        delta = current_frame - last_frame;
        last_frame = current_frame;


        // calculate the new Front vector and concluding right and up-vector
        // Ignore input, if in focus mode
        if (!m_in_focus_mode) {
            //handle input
            handle_input();

            // Camera Front Calculation
            if(m_mode == FLY)
            {
                glm::vec3 front;
                front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
                front.y = sin(glm::radians(m_pitch));
                front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
                m_camera_front = glm::normalize(front);
            }
            else if(m_mode == ORBIT)
            {
                position.x = radius * sin(glm::radians(phi)) * sin(glm::radians(theta));
                position.y = radius * cos(glm::radians(phi));
                position.z = radius * sin(glm::radians(phi)) * cos(glm::radians(theta));
                m_camera_front = glm::normalize(m_orbital_origin - position);
            }
        }
        m_camera_right = glm::normalize(glm::cross(m_camera_front, m_world_up));
        m_camera_up = glm::normalize(glm::cross(m_camera_right, m_camera_front));


        projection = glm::perspective(
                glm::radians(m_zoom),
                (float) m_screen_width / (float) m_screen_height,
                near,
                far
        );

        if (m_in_focus_mode) {
            // Calculate Coroutine
            camera_focus_coroutine();
        }
        // Calculate View Matrix with camera front vector offset as target
        view = glm::lookAt(
                position,
                position + m_camera_front,
                m_camera_up
        );

    }
    void Camera::handle_input()
    {

        ImVec2 vMin = ImGui::GetWindowContentRegionMin();
        ImVec2 vMax = ImGui::GetWindowContentRegionMax();
        vMin.x += ImGui::GetWindowPos().x;
        vMin.y += ImGui::GetWindowPos().y;
        vMax.x += ImGui::GetWindowPos().x;
        vMax.y += ImGui::GetWindowPos().y;

        // mouse movement
        auto xpos = (float)Input::get_mouse_X();
        auto ypos = (float)Input::get_mouse_Y();
        auto is_down = Input::mouse_pressed();

        if(!ImGui::IsWindowHovered() || !ImGui::IsWindowFocused())
        {
            if(!is_down)
            {
                last_x = xpos;
                last_y = ypos;
            }
            return;
        }

        // mouse scroll
        handle_mouse_scroll((float)Input::get_scroll_offset_Y());

        if(xpos > vMin.x && xpos < vMax.x && ypos > vMin.y && ypos < vMax.y)
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

                last_x = xpos;
                last_y = ypos;

                handle_mouse_movement(x_offset, y_offset);
            }
            else
            {
                last_x = xpos;
                last_y = ypos;
            }
        }

        // 3 Dimensional Movement
        if(ImGui::IsWindowFocused())
        {
            // X for Horizontal Movement
            // Y for Vertical Movement
            // Z for Forward Movement
            glm::vec3 input_vector = { Input::get_wasd_movement_vector_X() * m_horizontal_speed,
                                     Input::get_wasd_movement_vector_Y() * m_vertical_speed,
                                     Input::get_wasd_movement_vector_Z() * m_vertical_speed};
            input_vector *= delta;

            glm::vec3 mov_vector = input_vector.x * m_camera_right +
                    input_vector.y * m_camera_up +
                    input_vector.z * m_camera_front;

            position += mov_vector;
        }
    }

    void Camera::handle_mouse_scroll(float y_offset)
    {
        if(m_mode == FLY)
        {
            m_zoom -= m_zoom_strength * (float) y_offset;
            if(m_zoom < 1.0f)
            {
                m_zoom = 1.0f;
            }
            if(m_zoom > 90.0f)
            {
                m_zoom = 90.0f;
            }
        }
        if(m_mode == ORBIT)
        {
            radius -= (float) y_offset;
            if(radius < 1.0f)
            {
                radius = 1.0f;
            }
        }

    }

    void Camera::handle_mouse_movement(float x_offset, float y_offset)
    {
        if(m_mode == FLY)
        {
            x_offset *= m_sensitivity;
            y_offset *= m_sensitivity;

            m_yaw = std::fmod((m_yaw + x_offset), (float) 360.0f);
            m_pitch += y_offset;

            if (m_pitch > 89.0f)
            {
                m_pitch = 89.0f;
            }
            if (m_pitch < -89.0f)
            {
                m_pitch = -89.0f;
            }

        }
        if(m_mode == ORBIT)
        {
            x_offset *= m_sensitivity;
            y_offset *= m_sensitivity;

            phi += y_offset;
            if(phi < 1.0f)
            {
                phi = 1.0f;
            }
            if(phi > 179.0f)
            {
                phi = 179.0f;
            }

            theta -= x_offset;
            if(theta < 0.0f)
            {
                theta = 360.0f - (x_offset - theta);
            }
        }
    }

    void Camera::set_mode(int mode, float orbital_radius)
    {
        if(mode == 0)
        {
            m_mode = FLY;

            // Flying Mode
            m_pitch = asin(m_camera_front.y);
            m_yaw = acos((m_camera_front.x / cos(m_pitch)));

            m_pitch = glm::degrees(m_pitch);
            m_yaw = glm::degrees(m_yaw);

        }else if(mode == 1)
        {
            m_mode = ORBIT;

            // Orbital Mode
            position += m_orbital_origin;

            radius = orbital_radius;

            phi = acos(position.y / radius);
            theta = asin((position.x / radius) / sin(phi));

            phi = glm::degrees(phi);
            theta = glm::degrees(theta);

        }
    }

    void Camera::look_at(glm::vec3 target)
    {
        // Set Camera View Direction
        m_camera_front = target - position;

        // Change mode to existing mode ( fixes yaw and other angles )
        set_mode(m_mode, radius);
    }

    void Camera::camera_focus_coroutine()
    {
        float t = (m_target_mode_timer / m_target_mode_total_time);
        //std::cout << t << std::endl;
        if(t <= 0)
        {
            // Focusing Coroutine is done
            m_in_focus_mode = false;

            position = m_desired_position;
            m_camera_front = m_desired_front;

            // Set Variables depending on mode
            if(m_mode == FLY)
                set_mode(0);
            else
                set_mode(1, radius);
        }else{
            // Half Sinus Curve
            float logistic_t = (glm::sin((t * glm::pi<float>()) - glm::pi<float>()/2) + 1) / 2;

            position = glm::mix(m_desired_position, m_original_position, logistic_t);
            m_camera_front = glm::mix(m_desired_front, m_original_front, logistic_t);
        }

        // Reduce Timer
        m_target_mode_timer -= delta;
    }

    void Camera::focus_spot(glm::vec3 target_position, glm::vec3 target_normal)
    {
        // Set Target Mode to true
        m_in_focus_mode = true;
        m_orbital_origin = target_position;

        // Remember original data
        m_original_position = position;
        m_original_front = m_camera_front;
        // Set Desired Data
        m_desired_position = target_position + target_normal * radius;
        m_desired_front = -target_normal;

        m_target_mode_timer = m_target_mode_total_time;
        //std::cout << "Org Pos: " << VecUtil::to_string(m_original_position) << " Org Target: " << VecUtil::to_string(m_original_front) << std::endl;
        //std::cout << "Des Pos: " << VecUtil::to_string(m_desired_position) << " Des Target: " << VecUtil::to_string(m_desired_front) << std::endl;
    }
}