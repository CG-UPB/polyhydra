#pragma once

namespace volumeshOS::Internal
{
    class Renderer;

    struct RenderPass
    {
        virtual void render(const Renderer& renderer) = 0;
    };
}
