#pragma once

#include "polyhydra/polyhydraPCH.h"

namespace polyhydra::Internal
{
class Camera;

class TrackBall
{
  public:
    static glm::vec3
    get_rotation_axis(const glm::vec2& start_position, const glm::vec2& end_position, glm::vec2 viewport_size);
    static float
    get_rotation_angle(const glm::vec2& start_position, const glm::vec2& end_position, glm::vec2 viewport_size);

  private:
    static glm::vec3 get_trackball_vector(const glm::vec2& screen_pos, glm::vec2 viewport_size);
};
} // namespace polyhydra::Internal
