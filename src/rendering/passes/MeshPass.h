#pragma once

#include "RenderPass.h"

namespace volumeshOS::Internal
{

    class MeshPass : public RenderPass
    {
    public:

        MeshPass() = default;
        ~MeshPass() = default;

        void render(const Renderer& renderer) override;
    };
}