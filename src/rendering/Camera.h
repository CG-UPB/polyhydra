//
// Created by projektgruppe on 06.03.22.
//

#include "glm/gtx/transform.hpp"

#ifndef VOLUMESHOS_CAMERA_H
#define VOLUMESHOS_CAMERA_H



namespace vOS {


/**
 * Camera class responsible for Data concerning the rendering camera. Position direction and other attributes are managed and changable through this class
 */
    class Camera {

    public:
        Camera();

        glm::vec3 get_direction();

        void frame_update();

        void set_viewport_size(float width, float height);

        void update_matrices();

        void rotate_camera(glm::vec3 target);
        void rotate_camera(float yaw, float pitch);

        void handle_mouse_control();

        // Matrices
        glm::mat4 world;
        glm::mat4 view;
        glm::mat4 projection;

        // Vectors
        glm::vec3 position;
        glm::vec3 m_previous_position;
        glm::vec3 m_target;
        glm::vec3 m_previous_movement_vector;

        // Axis
        glm::vec3 m_world_up;
        glm::vec3 m_camera_right;
        glm::vec3 m_camera_up;
        glm::vec3 m_camera_front;

        // Euler Angles
        float m_yaw = 0;
        float m_pitch = 0;

        // Floats
        float m_movement_speed_multiplier = 1;
        float fov_deg = 50.0f;
        float near = 0.1f;
        float far = 100.0f;
        float m_screen_width = 0;
        float m_screen_height = 0;
        float m_zoom_strength = 0.1;
        float m_camera_movement_speed = 0.1f;
    };

}
#endif //VOLUMESHOS_CAMERA_H
