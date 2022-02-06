
#pragma once

#include "RenderPass.h"

namespace vOS
{

    class PrePass : public RenderPass
    {

    public:

        void render(VertexArrayObject* vao, const RenderData& data, int mesh_id) override;
    };
}
