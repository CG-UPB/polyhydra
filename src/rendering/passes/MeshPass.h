#pragma once

#include "RenderPass.h"
#include "../Renderer.h"

namespace volumeshOS::Internal
{
    class Renderer;

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