#pragma once

#include "vospch.h"

namespace vOS
{
    class Camera;

    class TrackBall
    {
    public:

        TrackBall(Camera& camera, float radius);

        void move_from_to(const glm::vec2& start_position, const glm::vec2& end_position);

        glm::mat4 get_transform() const;

    private:

        glm::vec3 get_trackball_vector(const glm::vec2& screen_pos);

        Camera& m_camera;
        float m_radius = 1.0f;
        glm::mat4 m_trackball_transform = glm::mat4(1.0f);

    };
}
