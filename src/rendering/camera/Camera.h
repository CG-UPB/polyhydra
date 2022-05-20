#pragma once

#include "vospch.h"

#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

#include "TrackBall.h"

namespace vOS {

    enum Mode
    {
        FLY,
        ORBIT
    };

/**
 * Camera class responsible for Data concerning the rendering camera. Position direction and other attributes are managed and changable through this class
 */
    class Camera {

    public:
        Camera();

        void set_viewport_size(float width, float height);
        void set_mode(Mode mode);

        glm::vec2 get_viewport_size() const;

        void update();

        void handle_input();
        void handle_mouse_scroll(float y_offset);
        void handle_mouse_movement(float x_offset, float y_offset);

        void look_at(glm::vec3 target);

        [[nodiscard]] const glm::vec3& get_front() const;
        [[nodiscard]] const glm::vec3& get_up() const;
        [[nodiscard]] const glm::vec3& get_right() const;

        // Matrices
        glm::mat4 world;
        glm::mat4 view;
        glm::mat4 projection;

        // Orientation
        glm::quat orientation;

        // Vectors
        glm::vec3 position = {0.0f, 0.0f , 0.0f};
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

    private:

        void handle_trackball_movement(const glm::vec2 start_position, const glm::vec2& end_position);

        TrackBall m_trackball;

        Mode m_mode = FLY;

        float m_vertical_speed = 6.0f;
        float m_horizontal_speed = 7.0f;
        float m_sensitivity = 0.2f;

        // Axis
        glm::vec3 m_world_up = {0,1,0};
        glm::vec3 m_camera_right = {0,0,0};
        glm::vec3 m_camera_up = {0,0,0};
        glm::vec3 m_camera_front= {0,0,0};

        float m_screen_width = 0;
        float m_screen_height = 0;
        float m_zoom_strength = 3.5;
    };

}
