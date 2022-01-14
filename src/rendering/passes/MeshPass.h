#pragma once

#include "RenderPass.h"

namespace vOS
{

    class MeshPass : public RenderPass
    {
    public:
        ~MeshPass() = default;

        void render(VertexArrayObject* vao, const RenderData& data, int mesh_id) override;
    private:
    };
}