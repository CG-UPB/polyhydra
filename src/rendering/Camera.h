//
// Created by projektgruppe on 06.03.22.
//

#include "glm/gtx/transform.hpp"

#ifndef VOLUMESHOS_CAMERA_H
#define VOLUMESHOS_CAMERA_H



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

        glm::vec3 get_viewport_size();

        void set_viewport_size(float width, float height);
        void set_mode(int mode, float orbital_radius = 10);

        void update();

        void handle_input();
        void handle_mouse_scroll(float y_offset);
        void handle_mouse_movement(float x_offset, float y_offset);

        void focus_spot(glm::vec3 target_position, glm::vec3 target_normal, float time);

        void look_at(glm::vec3 target);

        [[nodiscard]] const glm::vec3& get_front() const;
        [[nodiscard]] const glm::vec3& get_up() const;
        [[nodiscard]] const glm::vec3& get_right() const;

        // Matrices
        glm::mat4 world;
        glm::mat4 view;
        glm::mat4 projection;

        // Vectors
        glm::vec3 position = {0,0,0};
        glm::vec3 m_orbital_origin = {0,0,0};

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

        // Orbit
        float phi = 0;
        float theta = 0;
        float radius = 10;
        float zoom = 55.0f;

    private:

        Mode m_mode = FLY;

        glm::vec3 m_previous_position = {0,0,0};
        glm::vec3 m_previous_movement_vector = {0,0,0};

        float m_yaw = 0.0f;
        float m_pitch = 0.0f;
        float m_vertical_speed = 6.0f;
        float m_horizontal_speed = 7.0f;
        float m_sensitivity = 0.2f;
        /*
         * Moves the Camera one a step closer to a desired focus point with focus viewing direction
         * If timer runs through, the camera will look at the desired object and normal camera movement is allowed again
         */
        void camera_focus_coroutine();

        // Axis
        glm::vec3 m_world_up = {0,1,0};
        glm::vec3 m_camera_right = {0,0,0};
        glm::vec3 m_camera_up = {0,0,0};
        glm::vec3 m_camera_front= {0,0,0};

        bool m_in_focus_mode = false;
        float m_focus_mode_total_time = 2;
        float m_focus_mode_timer = 0.0f;
        glm::vec3 m_original_position = {0,0,0};
        glm::vec3 m_original_front = {0, 0, 0};
        glm::vec3 m_desired_front = {0, 0, 0};
        glm::vec3 m_desired_position = {0,0,0};

        float m_screen_width = 0;
        float m_screen_height = 0;
        float m_zoom_strength = 3.5;
    };

}
#endif //VOLUMESHOS_CAMERA_H
