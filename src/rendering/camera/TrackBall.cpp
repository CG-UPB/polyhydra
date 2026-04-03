
#include "polyhydra/rendering/camera/TrackBall.h"

#include "polyhydra//util/VecUtil.h"
#include "polyhydra/rendering/camera/Camera.h"

namespace polyhydra::Internal
{

glm::vec3
TrackBall::get_rotation_axis(const glm::vec2& start_position, const glm::vec2& end_position, glm::vec2 viewport_size)
{
    // get start and end position in camera space
    glm::vec3 v0 = get_trackball_vector(start_position, viewport_size);
    glm::vec3 v1 = get_trackball_vector(end_position, viewport_size);

    // get rotation axis
    glm::vec3 axis = glm::cross(v0, v1);

    return axis;
}

float TrackBall::get_rotation_angle(const glm::vec2& start_position,
                                    const glm::vec2& end_position,
                                    glm::vec2 viewport_size)
{
    glm::vec3 v0 = get_trackball_vector(start_position, viewport_size);
    glm::vec3 v1 = get_trackball_vector(end_position, viewport_size);
    float alpha = std::acos(std::min(1.0f, glm::dot(v0, v1)));

    return alpha;
}

glm::vec3 TrackBall::get_trackball_vector(const glm::vec2& screen_pos, glm::vec2 viewport_size)
{
    auto viewport_start = ImGui::GetCursorScreenPos();

    // get normalized screen coordinates
    auto p = glm::vec3((screen_pos.x - viewport_start.x) / viewport_size.x * 2.0f - 1.0f,
                       (screen_pos.y - viewport_start.y) / viewport_size.y * 2.0f - 1.0f,
                       0.0f);
    p.y = -p.y;
    // calculate position on trackball
    float r = 0.9f;

    auto d = sqrt(p.x * p.x + p.y * p.y);
    if (d <= (r / sqrt(2.0f)))
    {
        p.z = sqrt(r * r - d * d);
    }
    else
    {
        p.z = (r * r) / (2.0f * d);
    }
    p = glm::normalize(p);

    //        if (auto op_squared = p.x * p.x + p.y * p.y; op_squared <= r)
    //        {
    //            p.z = sqrt(r - op_squared);
    //        }
    //        else
    //        {
    //            // if clicked position is too far away from trackball, return the nearest point
    //            p = glm::normalize(p);
    //        }

    return p;
}
} // namespace polyhydra::Internal