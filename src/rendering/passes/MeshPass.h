#pragma once

#include "RenderPass.h"

namespace volumeshOS::Internal
{
    /**
     * Renders Meshes according to its set mesh properties
     */
    class MeshPass : public RenderPass
    {
    public:

        MeshPass() = default;
        ~MeshPass() = default;

        void render(const Renderer& renderer) override;
    };
}