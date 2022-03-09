#pragma once

#include "RenderPass.h"
#include "../../mesh/MeshObject.h"

namespace vOS
{
    /**
     * Renders a copy of whatever Element the user is currently hovering above
     */
    class SelectionHoverPass : public RenderPass
    {
    public:
        /**
         * Constructor
         */
        SelectionHoverPass();
        /**
         * Deconstructor
         */
        ~SelectionHoverPass();

        void render(VertexArrayObject* vao, const RenderData& data, int mesh_id) override;

        /**
         * Highlights given Element identified by ID, type and Mesh ID
         * @param mesh
         * @param data
         * @param mesh_id
         * @param type
         * @param id
         */
        void hover(const RenderData& data, int mesh_id, int type, int id);


        glm::vec3 m_zoom_point;
    private:

        /**
         * Data Object for Hover Information
         */
        struct HoverMeshData
        {
            std::vector<float> vertices;
            std::vector<unsigned int> indices;
        };

        /**
         * Create Face Mesh data from OVM Face
         * @param mesh
         * @param face_id
         * @return
         */
        HoverMeshData get_face_mesh_data(MeshObject& mesh, int face_id);
        // HoverMeshData get_edge_mesh_data(MeshObject& mesh, const RenderData& data, int mesh_id, int edge_id);
        // void add_vertex(const glm::vec3& vertex, std::vector<float>& vertices);

        // Helper Constants
        static const int SELECTION_TYPE_NONE = 0;
        static const int SELECTION_TYPE_VERTEX = 1;
        static const int SELECTION_TYPE_EDGE = 2;
        static const int SELECTION_TYPE_FACE = 3;

        // Hovered Element Information
        int m_hovered_type = -1;
        int m_hovered_id = -1;
        int m_hovered_mesh = -1;

        // Element Shaders
        Shader* m_flat_color_shader;
        Shader* m_quad_circle_shader;
        Shader* m_edge_hover_shader;
        // Hover Color
        glm::vec4 m_hover_color;
        // Hovered Element Information
        glm::vec4 m_hovered_vertex_position;
        glm::vec3 m_hovered_edge_from;
        glm::vec3 m_hovered_edge_to;

        // Vertex Array Objects
        VertexArrayObject* m_face_vao = nullptr;
        VertexArrayObject* m_quad_vao = nullptr;
        VertexArrayObject* m_edge_vao = nullptr;
    };
}