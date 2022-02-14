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
        void render(VertexArrayObject* vao, const RenderData& data, int mesh_id) override;

        void render_mesh(MeshObject* mesh, RenderData& data, int mesh_id);
        void set_debug_mode(bool mode);
        [[nodiscard]] bool is_debug_mode() const;

    private:

        bool m_debug = false;

        Shader* m_selection_shader;
        Shader* m_selection_sphere_shader;
        Shader* m_selection_cylinder_shader;

        VertexArrayObject* m_sphere_vao;
        VertexArrayObject* m_cylinder_vao;
        int m_num_vertices;
        int m_num_edges;
    };
}

