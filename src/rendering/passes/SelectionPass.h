#pragma once

#include "RenderPass.h"
#include "../../mesh/MeshObject.h"

namespace volumeshOS
{
    /**
     * Renders the entire scene again in which every element of every mesh is rendered in a unique
     * color according to its ID. The color can then be reversed back to an ID to make determining which element the user is hovering above easy to do
     */
    class SelectionPass : public RenderPass
    {
    public:

        SelectionPass();

        ~SelectionPass() = default;
        void render(std::shared_ptr<VertexArrayObject> vao, const RenderData& data, std::shared_ptr<MeshObject> mesh) override;

        /**
         * Renders specific Mesh
         * @param mesh
         * @param data
         * @param mesh_id
         */
        void render_mesh(std::shared_ptr<MeshObject> const& mesh, RenderData& data);
        void set_debug_mode(bool mode);
        [[nodiscard]] bool is_debug_mode() const;

    private:

        bool m_debug = false;

        // Shaders
        std::shared_ptr<Shader> m_selection_shader;
        std::shared_ptr<Shader> m_selection_sphere_shader;
        std::shared_ptr<Shader> m_selection_cylinder_shader;

        // Mesh Information
        int m_num_vertices;
        int m_num_edges;
        // VAOs
        std::shared_ptr<VertexArrayObject> m_sphere_vao;
        std::shared_ptr<VertexArrayObject> m_cylinder_vao;
    };
}

