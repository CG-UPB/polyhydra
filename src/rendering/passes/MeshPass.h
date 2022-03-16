#pragma once

#include "../Renderer.h"
#include "RenderPass.h"

namespace vOS
{
    class Renderer;

    /**
     * Renders Meshes according to its set mesh properties
     */
    class MeshPass : public RenderPass
    {
    public:

        explicit MeshPass(Renderer* renderer);
        /**
         * Deconstructor
         */
        ~MeshPass() = default;

        void render(VertexArrayObject* vao, const RenderData& data, int mesh_id) override;

    private:

        Renderer* m_renderer;
    };
}