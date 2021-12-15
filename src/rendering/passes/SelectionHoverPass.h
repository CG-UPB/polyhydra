#pragma once

#include "RenderPass.h"
#include "../../mesh/MeshObject.h"

namespace vOS
{
    class SelectionHoverPass : public RenderPass
    {
    public:

        SelectionHoverPass();
        ~SelectionHoverPass();

        void render(VertexArrayObject* vao, const RenderData& data, int mesh_id) override;

        void select(MeshObject& mesh, const RenderData& data, int mesh_id, int type, int id);

        glm::vec3 m_zoom_point;

    private:

        struct SelectionMeshData
        {
            std::vector<float> vertices;
            std::vector<unsigned int> indices;
        };

        SelectionMeshData get_face_mesh_data(MeshObject& mesh, int face_id);
        SelectionMeshData get_edge_mesh_data(MeshObject& mesh, const RenderData& data, int mesh_id, int edge_id);
        void add_vertex(const glm::vec3& vertex, std::vector<float>& vertices);

        static const int SELECTION_TYPE_NONE = 0;
        static const int SELECTION_TYPE_VERTEX = 1;
        static const int SELECTION_TYPE_EDGE = 2;
        static const int SELECTION_TYPE_FACE = 3;

        int m_selected_type = -1;
        int m_selected_id = -1;

        Shader* m_flat_color_shader;
        Shader* m_quad_circle_shader;
        Shader* m_edge_hover_shader;
        glm::vec4 m_hover_color;
        glm::vec4 m_selected_vertex_position;
        glm::vec3 m_selected_edge_from;
        glm::vec3 m_selected_edge_to;


        VertexArrayObject* m_face_vao = nullptr;
        VertexArrayObject* m_quad_vao = nullptr;
        VertexArrayObject* m_edge_vao = nullptr;
    };
}