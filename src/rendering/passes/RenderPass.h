#pragma once

namespace volumeshOS::Internal
{
    class Renderer;
    class Shader;

    struct RenderPass
    {
        virtual void render(const Renderer& renderer) = 0;
    };
}
