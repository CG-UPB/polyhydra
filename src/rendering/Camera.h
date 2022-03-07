//
// Created by projektgruppe on 06.03.22.
//

#include "glm/gtx/transform.hpp"

#ifndef VOLUMESHOS_CAMERA_H
#define VOLUMESHOS_CAMERA_H



namespace vOS {

    enum Movement
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

/**
 * Camera class responsible for Data concerning the rendering camera. Position direction and other attributes are managed and changable through this class
 */
    class Camera {

    public:
        Camera();

        void set_viewport_size(float width, float height);

        void update();

        void handle_input();
        void handle_mouse_scroll(float y_offset);
        void handle_mouse_movement(float x_offset, float y_offset);
        void handle_keyboard(Movement direction, float delta);

        // Matrices
        glm::mat4 world;
        glm::mat4 view;
        glm::mat4 projection;

        // Vectors
        glm::vec3 position;
        glm::vec3 m_target;
        glm::vec3 m_previous_position;
        glm::vec3 m_previous_movement_vector;

        // Axis
        glm::vec3 m_world_up;
        glm::vec3 m_camera_right;
        glm::vec3 m_camera_up;
        glm::vec3 m_camera_front;

        // Euler Angles
        float m_yaw = -90.0f;
        float m_pitch = 0.0f;
        float m_vertical_speed = 5.0f;
        float m_horizontal_speed = 10.0f;
        float m_sensitivity = 0.1f;
        float m_zoom = 55.0f;

        // Floats
        float near = 0.1f;
        float far = 100.0f;
        float m_screen_width = 0;
        float m_screen_height = 0;
        float m_zoom_strength = 1.5;

        // Mouse position
        float last_x = 0.0f;
        float last_y = 0.0f;
        bool first_mouse = true;

        float delta = 0.0f;
        float last_frame = 0.0f;
    };

}
#endif //VOLUMESHOS_CAMERA_H
