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

        void render(VertexArrayObject* vao, const RenderData& data) override;
        void render_mesh(MeshObject* mesh, const RenderData& data);

    private:

        Shader* m_selection_shader;
        Shader* m_selection_sphere_shader;
        Shader* m_selection_cylinder_shader;

        VertexArrayObject* m_sphere_vao;
        VertexArrayObject* m_cylinder_vao;
        int m_num_vertices;
        int m_num_edges;
    };
}

