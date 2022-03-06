#pragma once

#include "../gl/VertexArrayObject.h"
#include "../gl/Shader.h"
#include "../gl/FrameBufferObject.h"
#include "glm/gtx/transform.hpp"
#include "../Camera.h"

namespace vOS
{
    /**
     * Render Data includes World Information such as the Camera and Lighting
     */
    struct RenderData
    {
        struct Light {
            glm::vec3 color;
            glm::mat4 world;
            glm::mat4 view;
            glm::mat4 projection;
            glm::vec3 position;
        } light;
        struct {
            bool active = true;
            float size = 0.1f;
        } rounding;

        Camera camera;
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
