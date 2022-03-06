//
// Created by projektgruppe on 06.03.22.
//

#include "Camera.h"
#include "imgui.h"
#include "../input/Input.h"
#include <iostream>

namespace vOS {

    Camera::Camera() {

        set_viewport_size(1000,1000);

        // Init Position etc
        position = glm::vec3{0.0f, 0.0f, 10.0f};
        m_target = glm::vec3{0, 0, 0};

        // Axis
        m_world_up = glm::vec3(0,0,1);

        // set up the initial camera position, direction and orientation of the mesh
        world = glm::mat4(1.0f);

        // Update Projection and View Matrix
        update_matrices();
    }

    void Camera::set_viewport_size(float width, float height)
    {
        m_screen_width = width;
        m_screen_height = height;
    }

    glm::vec3 Camera::get_direction() {
        return glm::normalize(position - m_target);
    }

    void Camera::frame_update() {

        handle_mouse_control();

    }

    void Camera::update_matrices() {

        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        front.y = sin(glm::radians(m_pitch));
        front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_camera_front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        m_camera_right = glm::normalize(glm::cross(m_camera_front, m_world_up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        m_camera_up    = glm::normalize(glm::cross(m_camera_right, m_camera_front));

        m_camera_front = glm::normalize(position - m_target);
        m_camera_right = glm::vec3(1,0,0);
        m_camera_up = glm::vec3(0,1,0);

        projection = glm::perspective(
                glm::radians(fov_deg),
                (float) m_screen_width / (float) m_screen_height,
                near,
                far
        );
        view = glm::lookAt(
                position,
                m_target,
                m_camera_up
        );

        //std::cout << m_target[0] << " " << m_target[1] <<  " " << m_target[2] << std::endl;
    }

    void Camera::rotate_camera(glm::vec3 t)
    {
        m_target = t;
        update_matrices();
    }

    void Camera::rotate_camera(float yaw, float pitch)
    {
        m_yaw = yaw;
        m_pitch = pitch;

        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        front.y = sin(glm::radians(m_pitch));
        front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_camera_front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        m_camera_right = glm::normalize(glm::cross(m_camera_front, m_world_up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        m_camera_up    = glm::normalize(glm::cross(m_camera_right, m_camera_front));

        m_target = position + m_camera_front;
        update_matrices();
    }


    void Camera::handle_mouse_control() {


        // check where the imgui window is inside the main window, and how big it is
        ImVec2 vMin = ImGui::GetWindowContentRegionMin();
        ImVec2 vMax = ImGui::GetWindowContentRegionMax();
        vMin.x += ImGui::GetWindowPos().x;
        vMin.y += ImGui::GetWindowPos().y;
        vMax.x += ImGui::GetWindowPos().x;
        vMax.y += ImGui::GetWindowPos().y;
        glm::vec2 mousePos = {Input::get_mouse_X(), Input::get_mouse_Y()};

        if (!ImGui::IsWindowHovered()) {
            return;
        }

        bool isDown = Input::mouse_pressed();

        // Move camera in direction of Movement Vector (WASD movement)
        auto movement_vector = m_camera_right * Input::get_wasd_movement_vector_X() + m_camera_up * Input::get_wasd_movement_vector_Y() +
                                       -m_camera_front *  Input::get_wasd_movement_vector_Z();

        // Reset Movement speed multiplier whenever we stop moving or when we start moving
        if ((movement_vector[0] == 0 && movement_vector[1] == 0 && movement_vector[2] == 0) ||
            (m_previous_movement_vector[0] == 0 && m_previous_movement_vector[1] == 0 &&
             m_previous_movement_vector[2] == 0))
            m_movement_speed_multiplier = 1;

        m_previous_movement_vector[0] = movement_vector[0];
        m_previous_movement_vector[1] = movement_vector[1];
        m_previous_movement_vector[2] = movement_vector[2];

        float movement_speed = m_movement_speed_multiplier;
        m_movement_speed_multiplier *= 1.1f; // Gradually speed up movement
        m_movement_speed_multiplier = m_movement_speed_multiplier > 3 ? 3 : m_movement_speed_multiplier;
        m_previous_position = position;
        position += movement_vector * movement_speed * m_camera_movement_speed;

        if(m_previous_position != position)
            update_matrices();

        //std::cout << movement_vector[0] << " " << movement_vector[1] <<  " " << movement_vector[2] << std::endl;

        // Scroll Information

        auto direction = position - m_target;
        float strength = -Input::get_scroll_offset_Y();

        position += ( direction * (strength * m_zoom_strength));

        //std::cout <<  ( direction * (strength * 0.05f + 1))[0] << " " <<  ( direction * (strength * 0.05f + 1))[1] <<  " " <<  ( direction * (strength * 0.05f + 1))[2] << std::endl;


        if( Input::get_scroll_offset_Y() != 0)
            update_matrices();



        //std::cout << m_render_data.camera.position[0] << " "  << m_render_data.camera.position[1] << " " << m_render_data.camera.position[2] << " " << std::endl;

        // the cursor is inside the mesh viewport, so now we can manipulate the mesh view


        /*
        if (mousePos.x > vMin.x && mousePos.x < vMax.x && mousePos.y > vMin.y && mousePos.y < vMax.y) {
            // arc ball behavior
            if (isDown && !m_lastDown) {
                m_arcBallOn = true;
                m_lastX = mousePos.x;
                m_lastY = mousePos.y;
            }

            if (!isDown) {
                m_arcBallOn = false;
            }

            // scroll scaling of the mesh
            float scaleSpeed = 0.1f;
            m_render_data.camera.world = glm::scale(
                    m_render_data.camera.world,
                    glm::vec3(1.0f + (float) Input::get_scroll_offset_Y() * scaleSpeed)
            );
            m_render_data.light.world = glm::scale(
                    m_render_data.light.world,
                    glm::vec3(1.0f + (float) Input::get_scroll_offset_Y() * scaleSpeed)
            );
        }
        m_lastDown = isDown;

        if (m_arcBallOn) {
            float speed = 0.04;

            double dx = mousePos.x - m_lastX;
            double dy = mousePos.y - m_lastY;

            if (std::abs(dx) > 0.0 || std::abs(dy) > 0.0) {
                glm::vec3 a = get_arc_ball_vector((float) m_lastX, (float) m_lastY);
                glm::vec3 b = get_arc_ball_vector(mousePos.x, mousePos.y);
                float angle = (float) std::acos(std::min(1.0f, glm::dot(a, b)));
                glm::vec3 axis_camera = glm::cross(a, b);
                glm::mat3 camera_to_object = glm::inverse(
                        glm::mat3(m_render_data.camera.view) * glm::mat3(m_render_data.camera.world));
                glm::vec3 axis_object = camera_to_object * axis_camera;
                m_render_data.camera.world = glm::rotate(m_render_data.camera.world, glm::degrees(angle) * speed,
                                                         axis_object);


                glm::vec3 axis_light = glm::cross(a, b);
                glm::mat3 light_to_object = glm::inverse(
                        glm::mat3(m_render_data.light.view) * glm::mat3(m_render_data.light.world));
                glm::vec3 l_axis_object = light_to_object * axis_light;
                m_render_data.light.world = glm::rotate(m_render_data.light.world, glm::degrees(angle) * speed,
                                                        l_axis_object);
            }
        }
        m_lastX = mousePos.x;
        m_lastY = mousePos.y;
         */
    }

}