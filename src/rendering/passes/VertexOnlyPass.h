#pragma once

#include "RenderPass.h"

namespace volumeshOS
{

    class VertexOnlyPass : public RenderPass
    {

    public:

        VertexOnlyPass();

        void render(std::shared_ptr<VertexArrayObject> vao, const RenderData& data, std::shared_ptr<MeshObject> mesh) override;

    private:

        std::shared_ptr<Shader> m_vertex_only_shader = nullptr;
    };
}
