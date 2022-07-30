#pragma once

#include "RenderPass.h"
#include "rendering/gl/Shader.h"

namespace volumeshOS::Internal
{

    class VertexOnlyPass : public RenderPass
    {

    public:

        VertexOnlyPass();
        ~VertexOnlyPass() = default;

        void render(const Renderer& renderer) override;

    private:

        std::shared_ptr<Shader> m_vertex_only_shader = nullptr;
    };

}
