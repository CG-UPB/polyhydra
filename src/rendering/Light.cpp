
#include "polyhydra/rendering/Light.h"
#include "polyhydra/input/Input.h"

namespace polyhydra::Internal
{

Light::Light()
{
    position = glm::vec3{0.5f, 1.0f, 1.0f};
    color = glm::vec3{1.0f, 1.0f, 1.0f};
    target = glm::vec3{0.0f, 0.0f, 0.0f};
    light_dir = position;
    near = 0.1f;
    far = 100.0f;

    view = glm::lookAt(position, target, glm::vec3(0.0f, 1.0f, 0.0f));
    projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
    world = glm::mat4(1.0f);
}
} // namespace polyhydra::Internal