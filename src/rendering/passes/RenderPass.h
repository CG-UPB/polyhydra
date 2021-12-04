#pragma once

#include "../VertexArrayObject.h"
#include "../Shader.h"
#include "../FrameBufferObject.h"

#include "glm/gtx/transform.hpp"

namespace vOS
{
    struct RenderData
    {
        struct Camera {
            glm::mat4 world;
            glm::mat4 view;
            glm::mat4 projection;
            glm::vec3 position;
        } camera;
        struct Light {
            glm::vec3 position;
            glm::vec3 color;
        } light;
        struct Mesh {
            glm::vec3 offset;
            glm::mat4 transform;
            glm::vec3 color;
            int selection_offset;
        } mesh;
    };

    class RenderPass
    {
        virtual void render(VertexArrayObject* vao, const RenderData& data) = 0;
    };
}
