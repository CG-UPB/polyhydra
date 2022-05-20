#pragma once

#include "vospch.h"

#include "mesh/MeshObject.h"
#include "../gl/VertexArrayObject.h"
#include "../gl/FrameBufferObject.h"
#include "../gl/Shader.h"
#include "../Camera.h"
#include "../Light.h"

namespace vOS
{
    /**
     * Render Data includes World Information such as the Camera and Lighting
     */
    struct RenderData
    {
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
        virtual void render(VertexArrayObject* vao, const RenderData& render_data, std::shared_ptr<MeshObject> mesh) = 0;
    };
}
