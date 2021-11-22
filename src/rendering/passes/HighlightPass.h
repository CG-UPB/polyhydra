#pragma once

#include "RenderPass.h"

namespace vOS
{
    class HighlightPass : public RenderPass
    {
    public:
        HighlightPass();
        ~HighlightPass();

        void render(const VertexArrayObject& vao, const RenderData& data) override;

    private:

        VertexArrayObject* m_vao;
        Shader* m_highlight_shader;
    };
}
