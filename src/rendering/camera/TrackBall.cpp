
#include "TrackBall.h"

#include "../../util/VecUtil.h"
#include "Camera.h"

namespace vOS
{

    TrackBall::TrackBall(Camera& camera, float radius):
        m_camera(camera), m_radius(radius)
    {}

    void TrackBall::move_from_to(const glm::vec2& start_position, const glm::vec2& end_position)
    {
        // get start and end position in camera space
        glm::vec3 v0 = get_trackball_vector(start_position);
        glm::vec3 v1 = get_trackball_vector(end_position);

        // get rotation axis
        glm::vec3 axis = glm::cross(v0, v1);

        // get angle between v0, v1
        float alpha = std::acos(std::min(1.0f, glm::dot(v0, v1)));
//
//        glm::mat3 camera2object = glm::inverse(glm::mat3(transforms[MODE_CAMERA]) * glm::mat3(mesh.object2world));
//        glm::vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;

        // create rotation matrix
        float speed = 0.04f;
        float angle = glm::degrees(alpha) * speed;

        glm::mat3 camera_to_trackball = glm::inverse(glm::mat3(m_camera.view) * glm::mat3(m_camera.world));
        glm::vec3 axis_in_trackball_coords = camera_to_trackball * axis;

        m_camera.world = glm::rotate(m_camera.world, angle, axis_in_trackball_coords);
    }

    glm::vec3 TrackBall::get_trackball_vector(const glm::vec2& screen_pos)
    {
        auto viewport_start = ImGui::GetCursorScreenPos();

        // get normalized screen coordinates
        glm::vec2 viewport_size = m_camera.get_viewport_size();
        glm::vec3 p = glm::vec3((screen_pos.x - viewport_start.x) / viewport_size.x * 2.0f - 1.0f,
                                (screen_pos.y - viewport_start.y) / viewport_size.y * 2.0f - 1.0f,
                                0.0f);
        p.y = -p.y;
        // calculate position on trackball
        if(auto op_squared = p.x * p.x + p.y * p.y; op_squared <= 1.0f)
        {
            p.z = sqrt(1.0f - op_squared);
        }
        else
        {
            // if clicked position is too far away from trackball, return the nearest point
            p = glm::normalize(p);
        }
        return p;
    }

    glm::mat4 TrackBall::get_transform() const
    {
        return m_trackball_transform;
    }
}