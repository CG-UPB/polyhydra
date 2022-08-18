#pragma once

#include "RenderPass.h"

namespace volumeshOS::Internal
{

    class BackgroundPass : public RenderPass
    {
    public:

        BackgroundPass();

        void render(const Renderer& renderer) override;

    private:
        std::shared_ptr<Shader> m_background_shader = nullptr;
    };
}