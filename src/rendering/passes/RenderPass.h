#pragma once

#include "rendering/Renderer.h"

namespace volumeshOS::Internal
{
    struct RenderPass
    {
        virtual void render(const Renderer& renderer) = 0;
    };
}
