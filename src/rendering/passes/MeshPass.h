#pragma once

#include "../../panels/MeshView.h"
#include "RenderPass.h"

namespace vOS
{
    class MeshView;

    class MeshPass : public RenderPass
    {
    public:

        explicit MeshPass(MeshView* mesh_view);
        ~MeshPass() = default;

        void render(VertexArrayObject* vao, const RenderData& data, int mesh_id) override;

    private:

        MeshView* m_mesh_view;
    };
}