//
// Created by projektgruppe on 08.03.22.
//
#pragma once
#include "glm/gtx/transform.hpp"


namespace vOS {

    struct Ortho
    {
        Ortho(float x1, float x2, float y1, float y2, float z1, float z2)
        {
            min_x = x1;
            max_x = x2;
            min_y = y1;
            max_y = y2;
            min_z = z1;
            max_z = z2;
        }
        float min_x;
        float max_x;
        float min_y;
        float max_y;
        float min_z;
        float max_z;
    };

    class Light {

    public:
        Light();
        ~Light() = default;

        glm::mat4 world;
        glm::mat4 view;
        glm::mat4 projection;

        float near;
        float far;

        glm::vec3 position;
        glm::vec3 color;
        glm::vec3 light_dir;
        glm::vec3 target;

    private:
        glm::vec3 m_light_up;
        glm::vec3 m_light_front;
        glm::vec3 m_light_right;


    };

}