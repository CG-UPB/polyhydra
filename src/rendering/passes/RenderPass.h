#pragma once

#include "../gl/VertexArrayObject.h"
#include "../gl/Shader.h"
#include "../gl/FrameBufferObject.h"
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
            float fov_deg = 50.0f;
            float near = 0.1f;
            float far = 100.0f;
        } camera;
        struct Light {
            glm::vec3 position;
            glm::vec3 color;
        } light;
    };

    class RenderPass
    {
        virtual void render(VertexArrayObject* vao, const RenderData& render_data, int mesh_id) = 0;
    };
}
