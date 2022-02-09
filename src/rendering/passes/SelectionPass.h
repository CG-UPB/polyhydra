#pragma once

#include "RenderPass.h"
#include "../../mesh/MeshObject.h"

namespace vOS
{
    /**
     * Renders the entire scene again in which every element of every mesh is rendered in a unique
     * color according to its ID. The color can then be reversed back to an ID to make determining which element the user is hovering above easy to do
     */
    class SelectionPass : public RenderPass
    {
    public:

        // If true, the Viewer will render meshes the way the Selection Pass see's them
        static const bool DEBUG_MODE = false;

        /**
         * Constructor
         */
        SelectionPass();
        /**
         * Deconstructor
         */
        ~SelectionPass() = default;

        void render(VertexArrayObject* vao, const RenderData& data, int mesh_id) override;
        /**
         * Renders specific Mesh
         * @param mesh
         * @param data
         * @param mesh_id
         */
        void render_mesh(MeshObject* mesh, RenderData& data, int mesh_id);

    private:

        // Shaders
        Shader* m_selection_shader;
        Shader* m_selection_sphere_shader;
        Shader* m_selection_cylinder_shader;

        // Mesh Information
        int m_num_vertices;
        int m_num_edges;
        // VAOs
        VertexArrayObject* m_sphere_vao;
        VertexArrayObject* m_cylinder_vao;
    };
}

