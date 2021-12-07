#pragma once

#include "RenderPass.h"
#include "../../mesh/MeshObject.h"

namespace vOS
{
    class HighlightPass : public RenderPass
    {
    public:
        HighlightPass();
        ~HighlightPass();

        void render(VertexArrayObject* vao, const RenderData& data) override;

    private:

        VertexArrayObject* m_vao;
        Shader* m_highlight_shader;
    };
}
