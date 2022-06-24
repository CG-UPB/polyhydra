
#include "Camera.h"
#include "input/Input.h"
#include "../../util/VecUtil.h"

namespace volumeshOS {

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

    void Camera::handle_key_movement(glm::vec3 movement_vector)
    {
        movement_vector *= delta * 10;
        if (m_mode == FLY)
        {
            // Add the movement vector to the position
            glm::vec3 mov_vector = movement_vector.x * glm::normalize(get_right()) +
                                   movement_vector.y * glm::normalize(m_world_up) +
                                   movement_vector.z * glm::normalize(target - position);

            glm::vec3 view_dir = glm::normalize(target - position);
            position += mov_vector;
            target = position + view_dir;

        } else if (m_mode == ORBIT)
        {

        }
    }

    void Camera::handle_mouse_scroll(glm::vec2 scroll)
    {
        auto y_offset = scroll.y;
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
            auto step =  y_offset * glm::normalize(target - position);
            if(glm::length(target - (position + step) ) >= 1)
            {
                position += step;
            }
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

        float angle_x = std::min((float)M_PI / 10.0f, -x_offset * delta_angle_x);
        float angle_y = std::min((float)M_PI / 10.0f, y_offset * delta_angle_y);

        // when front and up are simalar
        float cos_angle = dot(get_front(), m_world_up);
        if (cos_angle * glm::sign(angle_y) > 0.99f)
        {
            angle_y = 0.0f;
            return;
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

    void Camera::switch_mode(glm::vec3 new_orbit_target)
    {
        if (m_mode == FLY)
        {
            auto extended_target = position + glm::length(glm::vec3(new_orbit_target) - position) *
                                                  glm::normalize(target - position);
            look_at(extended_target);
            set_mode(ORBIT);
            animated_look_at(new_orbit_target);
        }
        else if(m_mode == ORBIT)
        {
            set_mode(FLY);
            animated_look_at(position + glm::normalize(target - position));
        }
    }

    void Camera::set_mode(Mode mode)
    {
       m_mode = mode;
    }

    Mode Camera::get_mode()
    {
        return m_mode;
    }

    void Camera::animated_look_at(glm::vec3 new_target)
    {
        if(!animation)
        {
            animation = true;
            animation_start_target = target;
            animation_end_target = new_target;
            animation_start_position = position;
            auto pos_dir = glm::normalize(new_target - position);
            if(m_mode == ORBIT && glm::length(target - position) < glm::length(new_target -position))
            {
                animation_end_position = animation_end_target - pos_dir * glm::length(target - position);
            }
            else
            {
                animation_end_position = position;
            }
        }
    }

    void Camera::animation_step()
    {
        int steps = 15;
        if(animation)
        {
            glm::vec3 target_step = animation_end_target - animation_start_target ;
            target_step /= steps;

            glm::vec3 position_step = animation_end_position - animation_start_position;
            position_step /= steps;

            glm::vec3 remain = animation_end_target - target;

            if(glm::length(target_step) >= glm::length(remain))
            {
                position = animation_end_position;
                look_at(animation_end_target);
                animation = false;
            } else
            {
                position = position + position_step;
                look_at(target + target_step);
            }
        }
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
        return m_world_up;
    }

    glm::vec3 Camera::get_right() const
    {
        return glm::transpose(view)[0];
    }

}