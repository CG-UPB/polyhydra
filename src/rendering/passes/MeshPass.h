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

    private:

        float m_bias_min = 0.000005f;
        float m_bias_max = 0.0004f;
        float m_bias_modifier = 0.1f;
    };
}