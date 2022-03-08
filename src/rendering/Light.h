//
// Created by projektgruppe on 08.03.22.
//
#pragma once
#include "glm/gtx/transform.hpp"


namespace vOS {

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

        glm::vec3 m_target;
        glm::vec3 m_light_up;
        glm::vec3 m_light_front;
        glm::vec3 m_light_right;

        void update();
        void set_position( glm::vec3 pos);
        void set_target( glm::vec3 target);


    };

}