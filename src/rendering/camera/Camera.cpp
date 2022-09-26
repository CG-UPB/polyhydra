
#include "Camera.h"
#include "input/Input.h"
#include "../../util/VecUtil.h"
#include "settings/AppState.h"

namespace volumeshOS::Internal
{

    /**
     * 1) Trackball
     *
     */

    Camera::Camera()
    {

        set_viewport_size(800, 600);

        // Init Position etc
        position = glm::vec3{0.0f, 0.0f, 13.0f};
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
        m_vertical_speed = 1.5f;
        m_horizontal_speed = 1.5f;

        new_mode = CameraMode::FLY;
        set_mode(CameraMode::FLY);
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
        if(is_animating)
        {
            animation_step();
        }

        if(new_mode != m_mode)
        {
            m_mode = new_mode;
            switch_mode();
        }
        m_mode = new_mode;


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
        apply_changes();
    }

    void Camera::handle_key_movement(glm::vec3 movement_vector)
    {
        movement_vector *= delta * 10.0f;
        if (m_mode == CameraMode::FLY)
        {
            // Add the movement vector to the position
            glm::vec3 mov_vector = movement_vector.x * glm::normalize(get_right()) +
                                   movement_vector.y * glm::normalize(get_world_up()) +
                                   movement_vector.z * glm::normalize(glm::cross(get_world_up(), get_right()));

            glm::vec3 view_dir = glm::normalize(target - position);
            position += mov_vector;
            target = position + view_dir;

        } else if (m_mode == CameraMode::ORBIT)
        {

        }
    }

    void Camera::handle_mouse_scroll(glm::vec2 scroll)
    {
        auto y_offset = scroll.y;

        if (m_mode == CameraMode::FLY)
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
        } else if (m_mode == CameraMode::ORBIT)
        {
            auto len = glm::length(target - position);

            // adjust speed in Orbit Mode: the lower the distance from camera position to target
            auto dist_factor = std::clamp(len, min_distance, 1.0f);

            auto step =  y_offset * pow(dist_factor, 2.5f) * glm::normalize(target - position);

            auto new_pos = position + step;

            if(glm::length(target - new_pos ) <= min_distance || (glm::dot((target - position), step) >= 0 && glm::length(position - target) <= glm::length(position - new_pos)))
            {
                // keep minimal distance to target
                position =  target + min_distance * glm::normalize(position - target);
            }
            else
            {
                position = new_pos;
            }
        }
    }

    void Camera::handle_mouse_movement(float x_offset, float y_offset)
    {
        x_offset *= m_horizontal_speed ;
        y_offset *= m_vertical_speed ;

        glm::vec4 pos(position.x, position.y, position.z, 1.0f);
        glm::vec4 tgt(target.x, target.y, target.z, 1.0f);

        auto delta_angle_x = (float) (2 * M_PI / (m_screen_width));
        auto delta_angle_y = (float) (M_PI / (m_screen_height));

        float angle_x = -x_offset * delta_angle_x;
        float angle_y = y_offset * delta_angle_y;

        // when front and up are simalar
        float cos_angle = dot(get_front(), m_world_up);
        if (cos_angle * glm::sign(angle_y) > 0.99f)
        {
            angle_y = 0.0f;
        }

        if (m_mode == CameraMode::FLY)
        {
            glm::mat4 rotation_mat_x(1.0f);
            rotation_mat_x = glm::rotate(rotation_mat_x, angle_x, m_world_up);
            tgt = (rotation_mat_x * (pos - tgt)) + pos;

            glm::mat4 rotation_mat_y(1.0f);
            rotation_mat_y = glm::rotate(rotation_mat_y, angle_y, get_right());
            target = (rotation_mat_y * (pos - tgt)) + pos;

        } else if (m_mode == CameraMode::ORBIT)
        {
            glm::mat4 rotation_mat_x(1.0f);
            rotation_mat_x = glm::rotate(rotation_mat_x, angle_x, m_world_up);
            pos = (rotation_mat_x * (pos - tgt)) + tgt;

            glm::mat4 rotation_mat_y(1.0f);
            rotation_mat_y = glm::rotate(rotation_mat_y, angle_y, get_right());
            position = (rotation_mat_y * (pos - tgt)) + tgt;
        }
    }

    void Camera::switch_mode()
    {
        if (m_mode == CameraMode::ORBIT)
        {
            auto extended_target = position + glm::length(target - position) * glm::normalize(old_target - position);
            look_at(extended_target);
            animated_look_at(target);
        }
        else if(m_mode == CameraMode::FLY)
        {
            //look_at(position + glm::normalize((target - position)));
            //animated_look_at(position + glm::normalize(old_target - position));
        }
    }

    void Camera::set_mode(CameraMode mode)
    {
        new_mode = mode;
    }

    CameraMode Camera::get_mode()
    {
        return m_mode;
    }

    void Camera::animated_look_at(glm::vec3 tgt)
    {
        if(!is_animating)
        {
            is_animating = true;
            animation.time_start = (float)ImGui::GetTime();
            animation.time_current = animation.time_start;
            animation.time_end = animation.time_start + animation.duration;

            animation.target_start = position + glm::normalize(target - position) * glm::length(tgt - position);
            animation.target_end = tgt;

            animation.position_start = position;
            auto pos_dir = glm::normalize(tgt - position);
            if(m_mode == CameraMode::ORBIT && glm::length(target - position) < glm::length(tgt -position))
            {
                animation.position_end = animation.target_end - pos_dir * glm::length(target - position);
            }
            else
            {
                //animation.target_end = position + glm::normalize(tgt - position);
                animation.position_end = position;
            }
        }
    }

    void Camera::animated_look_at(glm::vec3 tgt, glm::vec3 pos)
    {
        if(!is_animating)
        {
            is_animating = true;
            animation.time_start = (float)ImGui::GetTime();
            animation.time_current = animation.time_start;
            animation.time_end = animation.time_start + animation.duration;

            animation.target_start = position + glm::normalize(target - position) * glm::length(tgt - position);
            animation.target_end = tgt;

            animation.position_start = position;
            auto pos_dir = glm::normalize(tgt - position);
            if(glm::length(pos - position) >= min_distance)
            {
                animation.position_end = pos;
            }
            else
            {
                animation.position_end = position;
            }
        }
    }

    void Camera::animation_step()
    {
        if(is_animating)
        {
            auto factor = ((animation.time_current - animation.time_start) / (animation.time_end - animation.time_start));
            animation.time_current += delta;

            glm::vec3 target_step = smoothstep(factor) *  (animation.target_end - animation.target_start) ;
            glm::vec3 position_step = smoothstep(factor) * (animation.position_end - animation.position_start);

            if(animation.time_current > animation.time_end)
            {
                position = animation.position_end;
                look_at(animation.target_end);
                is_animating = false;
            } else
            {
                position = animation.position_start + position_step;
                look_at(animation.target_start + target_step);
            }
        }
    }

    void Camera::look_at(glm::vec3 tgt)
    {
        old_target = target;
        target = tgt;
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
        return glm::transpose(view)[1];
    }

    glm::vec3 Camera::get_world_up() const
    {
        return m_world_up;
    }

    glm::vec3 Camera::get_right() const
    {
        return glm::transpose(view)[0];
    }

    void Camera::set_position(const glm::vec3 pos)
    {
        position = pos;
    }

    glm::vec3 Camera::get_position() const
    {
        return position;
    }

    void Camera::set_target(const glm::vec3 tgt)
    {
        old_target = target;
        target = tgt;
    }

    glm::vec3 Camera::get_target() const
    {
        return target;
    }

    void Camera::apply_changes() const
    {
        AppState::settings.camera.mode = m_mode;
        AppState::settings.camera.position = position;
        AppState::settings.camera.fov = zoom;
    }

    float Camera::smoothstep(float x)
    {
        return x * x * (3 - 2 * x);
    }

}