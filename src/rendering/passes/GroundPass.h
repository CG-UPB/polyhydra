#pragma once

#include "RenderPass.h"

namespace volumeshOS::Internal
{
    /**
     * Renders the Background visible when no other Mesh occupies the Viewfield
     */
    class GroundPass : public RenderPass
    {
    public:

        GroundPass();

        void render(const Renderer& renderer) override;

    private:

        // A simple plane mesh that is always visible but behind all other objects
        std::unique_ptr<VertexArrayObject> m_vao;
        // Simple gradient shader
        std::shared_ptr<Shader> m_ground_shader;
    };
}