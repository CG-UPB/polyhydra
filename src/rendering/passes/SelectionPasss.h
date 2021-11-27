#pragma once

#include "RenderPass.h"

namespace vOS
{
    class SelectionPass : public RenderPass
    {
    public:
        SelectionPass();
        ~SelectionPass() = default;

        void render(const VertexArrayObject& vao, const RenderData& data) override;

    private:

        Shader* m_selection_shader;
    };
}

