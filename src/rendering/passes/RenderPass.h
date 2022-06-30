#pragma once

#include "vospch.h"

#include "mesh/MeshObject.h"
#include "../gl/VertexArrayObject.h"
#include "../gl/FrameBufferObject.h"
#include "../gl/Shader.h"
#include "rendering/camera/Camera.h"
#include "../Light.h"
#include "rendering/Renderer.h"

namespace volumeshOS::Internal
{
    class RenderPass
    {
    public:

        explicit RenderPass(Renderer* renderer): m_renderer(renderer)
        {}

        /**
         * Renders the VAO using render data and mesh data as parameters
         * @param vao
         * @param render_data
         * @param mesh_id
         */
        virtual void render(std::shared_ptr<VertexArrayObject> vao, std::shared_ptr<MeshObject> mesh) = 0;

    protected:

        Renderer* m_renderer = nullptr;
    };
}
