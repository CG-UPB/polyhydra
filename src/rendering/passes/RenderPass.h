#pragma once

#include "../gl/VertexArrayObject.h"
#include "../gl/Shader.h"
#include "../gl/FrameBufferObject.h"
#include "glm/gtx/transform.hpp"
#include "../Camera.h"
#include "../Light.h"

namespace vOS
{
    /**
     * Render Data includes World Information such as the Camera and Lighting
     */
    struct RenderData
    {
        struct {
            bool active = true;
            float size = 0.3f;
        } rounding;
        Camera camera;
        Light light;
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
