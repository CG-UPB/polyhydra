#pragma once

#include "../gl/VertexArrayObject.h"
#include "../gl/Shader.h"
#include "../gl/FrameBufferObject.h"
#include "glm/gtx/transform.hpp"

namespace vOS
{
    /**
     * Render Data includes World Information such as the Camera and Lighting
     */
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
    };

    class RenderPass
    {
        /**
         * Renders the VAO using render data and mesh data as parameters
         * @param vao
         * @param render_data
         * @param mesh_id
         */
        virtual void render(VertexArrayObject* vao, const RenderData& render_data, int mesh_id) = 0;
    };
}
