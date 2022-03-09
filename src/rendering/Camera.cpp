//
// Created by projektgruppe on 06.03.22.
//

#include "Camera.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "../input/Input.h"
#include <iostream>

namespace vOS
{

    Camera::Camera()
    {

        set_viewport_size(800, 600);

        // Init Position etc
        position = glm::vec3{0.0f, 0.0f, 10.0f};
        m_target = glm::vec3{0.0f, 0.0f, 0.0f};
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
        //handle input
        handle_input();

        if(m_mode == FLY)
        {
            glm::vec3 front;
            front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
            front.y = sin(glm::radians(m_pitch));
            front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
            m_camera_front = glm::normalize(front);
        }
        if(m_mode == ORBIT)
        {
            position.x = radius * sin(glm::radians(phi)) * sin(glm::radians(theta));
            position.y = radius * cos(glm::radians(phi));
            position.z = radius * sin(glm::radians(phi)) * cos(glm::radians(theta));
            m_camera_front = glm::normalize(m_target - position);
        }

        // calculate the new Front vector and concluding right and up-vector

        m_camera_right = glm::normalize(glm::cross(m_camera_front, m_world_up));
        m_camera_up = glm::normalize(glm::cross(m_camera_right, m_camera_front));

        projection = glm::perspective(
                glm::radians(m_zoom),
                (float) m_screen_width / (float) m_screen_height,
                near,
                far
        );
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

        // keyboard
        auto current_frame = (float)ImGui::GetTime();
        delta = current_frame - last_frame;
        last_frame = current_frame;

        if(ImGui::IsKeyDown('W'))
        {
            handle_keyboard(FORWARD, delta);
        }
        if(ImGui::IsKeyDown('A'))
        {
            handle_keyboard(LEFT, delta);
        }
        if(ImGui::IsKeyDown('S'))
        {
            handle_keyboard(BACKWARD, delta);
        }
        if(ImGui::IsKeyDown('D'))
        {
            handle_keyboard(RIGHT, delta);
        }

        if(ImGui::IsKeyPressed('M'))
        {
            if(m_mode == ORBIT)
            {
                m_pitch = asin(m_camera_front.y);
                m_pitch = glm::degrees(m_pitch);
                m_yaw = 270.0f - theta;

                if (m_pitch > 89.0f)
                {
                    m_pitch = 89.0f;
                }
                if (m_pitch < -89.0f)
                {
                    m_pitch = -89.0f;
                }

                set_mode(FLY);
            }
            else if(m_mode == FLY)
            {
                phi = acos(position.y / radius) ;
                theta = acos( (position.z / radius) / sin(phi)) ;
                phi = glm::degrees(phi);
                theta = glm::degrees(theta);
                theta = 360.0f - (m_yaw - 270.0f);

                radius = glm::length(position - m_target);

                set_mode(ORBIT);
            }
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

            if(m_yaw < 0.0f)
            {
                m_yaw = 360.0f - (x_offset -  m_yaw);
            }

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
            std::cout << "Pitch: " << m_pitch << " ,Yaw: " << m_yaw << std::endl;

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

    void Camera::handle_keyboard(Movement direction, float delta)
    {
        if(m_mode == FLY)
        {
            float velocity = 0.0;
            if (direction == FORWARD)
            {
                velocity = m_vertical_speed * delta;
                position += m_camera_front * velocity;
            }
            if (direction == BACKWARD)
            {
                velocity = m_vertical_speed * delta;
                position -= m_camera_front * velocity;
            }
            if (direction == LEFT)
            {
                velocity = m_horizontal_speed * delta;
                position -= m_camera_right * velocity;
            }
            if (direction == RIGHT)
            {
                velocity = m_horizontal_speed * delta;
                position += m_camera_right * velocity;
            }
        }
        if(m_mode == ORBIT)
        {
            float velocity = 2.0;
            if (direction == FORWARD)
            {
                phi += velocity;
            }
            if (direction == BACKWARD)
            {
                phi -= velocity;
            }
            if(phi < 1.0f)
            {
                phi = 1.0f;
            }
            if(phi > 179.0f)
            {
                phi = 179.0f;
            }

            if (direction == LEFT)
            {
                theta -= velocity;
            }
            if (direction == RIGHT)
            {
                theta += velocity;
            }
            if(theta < 0.0f)
            {
                theta = 360.0f - (velocity - theta);
            }
        }
    }
}