#pragma once

#include "RenderPass.h"
#include "../../mesh/MeshObject.h"

namespace vOS
{
    class SelectionPass : public RenderPass
    {
    public:
        SelectionPass();
        ~SelectionPass() = default;

        void render(const VertexArrayObject& vao, const RenderData& data) override;
        void render_mesh(MeshObject* mesh, const RenderData& data);

    private:

        Shader* m_selection_shader;
        Shader* m_selection_shape_shader;

        VertexArrayObject* m_sphere_vao;
        int m_num_vertices;
    };
}

