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
        m_camera_front = glm::vec3{0.0f, 0.0f, -1.0f};
        m_camera_up = glm::vec3{0.0f, 1.0f, 0.0f};

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

        set_mode(FLY);
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

        // Frame Delta
        auto current_frame = (float) ImGui::GetTime();
        delta = current_frame - last_frame;
        last_frame = current_frame;


        // calculate the new Front vector and concluding right and up-vector
        // Ignore input, if in focus mode
        if (!m_in_focus_mode)
        {
            //handle input
            handle_input();

            // Camera Front Calculation
            if (m_mode == FLY)
            {
                glm::vec3 front;
                front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
                front.y = sin(glm::radians(m_pitch));
                front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
                m_camera_front = glm::normalize(front);
            } else if (m_mode == ORBIT)
            {
                //std::cout << phi << " " << theta << std::endl;
                // Orbit changes the position directly
                position.x = radius * sin(glm::radians(phi)) * sin(glm::radians(theta));
                position.y = radius * cos(glm::radians(phi));
                position.z = radius * sin(glm::radians(phi)) * cos(glm::radians(theta));
                position += m_orbital_origin;
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

        if (m_in_focus_mode)
        {
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
        // If left ctrl key is pressed, the object move mode is activbe which requires the camera stand still
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
            glm::vec3 input_vector = {Input::get_wasd_movement_vector_X() * m_horizontal_speed,
                                      Input::get_wasd_movement_vector_Y() * m_vertical_speed,
                                      Input::get_wasd_movement_vector_Z() * m_vertical_speed};
            input_vector *= delta;
            if (m_mode == FLY)
            {
                // Add the movement vector to the position
                glm::vec3 mov_vector = input_vector.x * m_camera_right +
                                       input_vector.y * m_camera_up +
                                       input_vector.z * m_camera_front;

                position += mov_vector;
                //std::cout <<  "moving" << VecUtil::to_string(mov_vector) << std::endl;
            } else if (m_mode == ORBIT)
            {
                // Add the input vector to phi and theta coordinates
                float velocity = 20.0;
                if (input_vector.z != 0)
                {
                    phi += input_vector.z * velocity;
                    if (phi < 1.0f)
                        phi = 1.0f;
                    else if (phi > 179.0f)
                        phi = 179.0f;
                }
                if (input_vector.x != 0)
                {
                    theta += input_vector.x * velocity;
                    if (theta < 0.0f)
                        theta = 360.0f - (velocity - theta);
                }
            }
        }
    }

    void Camera::handle_mouse_scroll(float y_offset)
    {
        if (m_mode == FLY)
        {
            m_zoom -= m_zoom_strength * (float) y_offset;
            if (m_zoom < 1.0f)
            {
                m_zoom = 1.0f;
            }
            if (m_zoom > 90.0f)
            {
                m_zoom = 90.0f;
            }
        }
        if (m_mode == ORBIT)
        {
            radius -= (float) y_offset;
            if (radius < 1.0f)
            {
                radius = 1.0f;
            }
        }
    }

    void Camera::handle_mouse_movement(float x_offset, float y_offset)
    {
        if (m_mode == FLY)
        {
            x_offset *= m_sensitivity;
            y_offset *= m_sensitivity;

            if (m_yaw < 0.0f)
            {
                m_yaw = 360.0f - (x_offset - m_yaw);
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
        if (m_mode == ORBIT)
        {
            x_offset *= m_sensitivity;
            y_offset *= m_sensitivity;

            phi += y_offset;
            if (phi < 1.0f)
            {
                phi = 1.0f;
            }
            if (phi > 179.0f)
            {
                phi = 179.0f;
            }

            theta -= x_offset;
            if (theta < 0.0f)
            {
                theta = 360.0f - (x_offset - theta);
            }
        }
    }

    void Camera::set_mode(int mode, float orbital_radius)
    {
        if (mode == 0)
        {
            m_mode = FLY;

            m_camera_front = glm::normalize(m_camera_front);
            //std::cout << VecUtil::to_string(m_camera_front) << " initial " << std::endl;
            // Flying Mode
            m_pitch = asin(m_camera_front.y);
            float acos_product = (m_camera_front.x / cos(m_pitch));
            acos_product = (acos_product <= -1 ? -0.999f : (acos_product >= 1 ? 0.999f : acos_product));
            m_yaw = acos(acos_product);

            m_pitch = glm::degrees(m_pitch);
            m_yaw = glm::degrees(m_yaw);

            // Test the yaw and pitch
            glm::vec3 front;
            front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
            front.y = sin(glm::radians(m_pitch));
            front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));


            // If the resulting test direction is not like the initial direction, we rotate yaw by 180
            if(glm::length(front - m_camera_front) > 0.1f) {
                //std::cout << "deploying change" << std::endl;
                m_yaw += 180;
            }
            //std::cout << VecUtil::to_string(front) << " test " << std::endl;
            // Another problem occus when both pitch is 90 degress, in which case rotation would screw up because of the missing roll variable
            m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);
            // std::cout << m_pitch << " " << m_yaw << std::endl;
        }else if(mode == 1)
        {
            m_mode = ORBIT;

            // Orbital Mode
            //position += m_orbital_origin;


            glm::vec3 sphere_direction = position - m_orbital_origin;
            if (m_orbital_origin == position)
                sphere_direction = {0, 0, -1};
            else
                sphere_direction = glm::normalize(sphere_direction);

            radius = orbital_radius;

            phi = acos(sphere_direction.y);
            phi = glm::degrees(phi);
            if (phi < 1.0f)
                phi = 1.0f;
            else if (phi > 179.0f)
                phi = 179.0f;
            theta = asin((sphere_direction.x) / sin(glm::radians(phi)));

            theta = glm::degrees(theta);
            if (theta < 0.0f)
                theta = 360.0f - theta;

            // Orbit changes the position directly
            position.x = radius * sin(glm::radians(phi)) * sin(glm::radians(theta));
            position.y = radius * cos(glm::radians(phi));
            position.z = radius * sin(glm::radians(phi)) * cos(glm::radians(theta));
            position += m_orbital_origin;
            m_camera_front = glm::normalize(m_orbital_origin - position);
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
        float t = (m_focus_mode_timer / m_focus_mode_total_time);

        if (t <= 0)
        {
            // Focusing Coroutine is done
            m_in_focus_mode = false;

            position = m_desired_position;
            m_camera_front = m_desired_front;

            // Set Variables depending on mode
            if (m_mode == FLY)
                set_mode(0);
            else
                set_mode(1, radius);
        } else
        {
            // Half Sinus Curve
            float logistic_t = (glm::sin((t * glm::pi<float>()) - glm::pi<float>() / 2) + 1) / 2;

            position = glm::mix(m_desired_position, m_original_position, logistic_t);
            m_camera_front = glm::mix(m_desired_front, m_original_front, logistic_t);
        }

        // Reduce Timer
        m_focus_mode_timer -= delta;
    }

    void Camera::focus_spot(glm::vec3 target_position, glm::vec3 target_normal, float time)
    {
        //std::cout << VecUtil::to_string(target_position) << " " << VecUtil::to_string(target_normal) << std::endl;

        // Switch to Focus Mode
        m_in_focus_mode = true;
        m_orbital_origin = target_position;
        m_focus_mode_total_time = time;

        // Remember original data
        m_original_position = position;
        m_original_front = m_camera_front;
        // Set Desired Data
        m_desired_front = -target_normal;

        // We must avoid situations where the  pitch will be outside of a 89 degree bound
        if(abs(m_desired_front.y) > 0.9)
        {
            m_desired_front.x = 0.12;
        }
        m_desired_front = glm::normalize(m_desired_front);

        m_desired_position = target_position + -m_desired_front * radius;
        m_focus_mode_timer = m_focus_mode_total_time;
        //std::cout << "Org Pos: " << VecUtil::to_string(m_original_position) << " Org Target: " << VecUtil::to_string(m_original_front) << std::endl;
        //std::cout << "Des Pos: " << VecUtil::to_string(m_desired_position) << " Des Target: " << VecUtil::to_string(m_desired_front) << std::endl;
    }

    glm::vec3 Camera::get_viewport_size()
    {
        return {m_screen_width, m_screen_height, 1};
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