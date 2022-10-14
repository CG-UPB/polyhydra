#pragma once

#include "vospch.h"

#include "RenderPass.h"

namespace volumeshOS::Internal
{
    class OutlinePass : public RenderPass
    {
    public:
        OutlinePass();

        void render(const Renderer& renderer) override;

    private:
        std::shared_ptr<Shader> m_shader = nullptr;
    };
}
