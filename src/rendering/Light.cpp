//
// Created by projektgruppe on 08.03.22.
//

#include "Light.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "../input/Input.h"
#include <iostream>

namespace vOS
{

    Light::Light()
    {
        position =glm::vec3{0.0f, 15.0f, 10.0f};
        color = glm::vec3{1.0f, 1.0f, 1.0f};
        m_target = glm::vec3{0.0f, 0.0f, 0.0f};

        near = 0.1f;
        far = 100.0f;

        world = glm::mat4(1.0f);

        projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);

        view = glm::lookAt(
                position,
                m_target,
                glm::vec3(0.0f, 1.0f, 0.0f)
        );
    }


    void Light::update()
    {
        projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);

        view = glm::lookAt(
                position,
                m_target,
                glm::vec3(0.0f, 1.0f, 0.0f)
        );
    }

    void Light::set_position(glm::vec3 pos)
    {
        position = pos;
    }

    void Light::set_target(glm::vec3 target)
    {
        m_target = target;
    }
}