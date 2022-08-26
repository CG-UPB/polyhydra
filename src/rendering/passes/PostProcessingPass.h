#pragma once

#include "vospch.h"
#include "RenderPass.h"

namespace volumeshOS::Internal
{
    class PostProcessingPass : public RenderPass
    {
    public:

        PostProcessingPass();
        ~PostProcessingPass() = default;

        void render(const Renderer& renderer) override;

    private:
        std::shared_ptr<Shader> m_post_processing_shader = nullptr;
    };
}