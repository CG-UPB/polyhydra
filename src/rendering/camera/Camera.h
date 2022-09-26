#pragma once

#include "vospch.h"

#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "util/Enums.h"

#include "TrackBall.h"

namespace volumeshOS::Internal
{
    struct Animation{
        glm::vec3 target_start   = {0.0f, 0.0f , 0.0f};
        glm::vec3 target_end     = {0.0f, 0.0f , 0.0f};
        glm::vec3 position_start = {0.0f, 0.0f , 0.0f};
        glm::vec3 position_end   = {0.0f, 0.0f , 0.0f};
        float duration           = 0.4f;
        float time_start         = 0.0f;
        float time_current       = 0.0f;
        float time_end           = 0.0f;
    };

/**
 * Camera class responsible for Data concerning the rendering camera. Position direction and other attributes are managed and changable through this class
 */
    class Camera {

    public:
        Camera();

        void set_viewport_size(float width, float height);

        void switch_mode();

        [[nodiscard]] glm::vec2 get_viewport_size() const;

        void update();
        void handle_mouse_scroll(glm::vec2 scroll);
        void handle_mouse_movement(float x_offset, float y_offset);
        void handle_key_movement(glm::vec3 movement_vector);
        void apply_changes() const;

        void animated_look_at(glm::vec3 new_target);
        void animated_look_at(glm::vec3 new_target, glm::vec3 new_position);
        void animation_step();
        void look_at(glm::vec3 new_target);

        [[nodiscard]] glm::vec3 get_front() const;
        [[nodiscard]] glm::vec3 get_world_up() const;
        [[nodiscard]] glm::vec3 get_up() const;
        [[nodiscard]] glm::vec3 get_right() const;

        void set_position(glm::vec3 position);
        glm::vec3 get_position() const;
        void set_target(glm::vec3 target);
        glm::vec3 get_target() const;
        void set_mode(CameraMode mode);
        CameraMode get_mode();

        // Matrices
        glm::mat4 world;
        glm::mat4 view;
        glm::mat4 projection;

        // Orientation
        glm::quat orientation;

        // Vectors
        glm::vec3 position = {0.0f, 0.0f , 13.0f};
        glm::vec3 target = {0.0f, 0.0f , 0.0f};

        // Floats
        float near = 0.1f;
        float far = 100.0f;

        // Mouse position
        float last_x = 0.0f;
        float last_y = 0.0f;
        bool first_mouse = true;

        // Movement
        float delta = 0.0f;
        float last_frame = 0.0f;

        float zoom = 45.0f;

        float min_distance = 0.1f;

        bool is_animating = false;
        bool blocking = false;

    private:

        CameraMode m_mode = CameraMode::FLY;
        CameraMode new_mode = CameraMode::FLY;

        float m_vertical_speed = 0.5f;
        float m_horizontal_speed = 0.5f;
        float m_sensitivity = 0.2f;

        // Axis
        glm::vec3 m_world_up = {0,1,0};
        glm::vec3 m_camera_right = {0,0,0};
        glm::vec3 m_camera_up = {0,0,0};
        glm::vec3 m_camera_front= {0,0,0};
        glm::vec3 old_target = {0, 0, 0};

        float m_screen_width = 0;
        float m_screen_height = 0;
        float m_zoom_strength = 3.5;

        Animation animation;

        float smoothstep(float x);

    };

}
