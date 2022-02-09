#pragma once

#include "RenderPass.h"

namespace vOS
{
    /**
     * Renders Meshes according to its set mesh properties
     */
    class MeshPass : public RenderPass
    {
    public:
        /**
         * Deconstructor
         */
        ~MeshPass() = default;

        void render(VertexArrayObject* vao, const RenderData& data, int mesh_id) override;
    private:
    };
}