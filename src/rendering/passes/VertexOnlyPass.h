#pragma once

#include "RenderPass.h"

namespace vOS
{

    class VertexOnlyPass : public RenderPass
    {

    public:

        VertexOnlyPass();

        void render(VertexArrayObject* vao, const RenderData& data, int mesh_id) override;

    private:

        Shader* m_vertex_only_shader = nullptr;
    };
}
