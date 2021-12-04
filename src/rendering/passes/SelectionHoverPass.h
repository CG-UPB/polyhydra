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

        void render(VertexArrayObject* vao, const RenderData& data) override;

        void select(MeshObject& mesh, int type, int id);

    private:

        struct MeshData
        {
            std::vector<float> vertices;
            std::vector<unsigned int> indices;
        };

        MeshData get_face_mesh_data(MeshObject& mesh, int face_id);
        MeshData get_edge_mesh_data(MeshObject& mesh, int edge_id);
        void add_vertex(const glm::vec3& vertex, std::vector<float>& vertices);

        static const int SELECTION_TYPE_NONE = 0;
        static const int SELECTION_TYPE_VERTEX = 1;
        static const int SELECTION_TYPE_EDGE = 2;
        static const int SELECTION_TYPE_FACE = 3;

        int m_selected_type = -1;
        int m_selected_id = -1;

        Shader* m_hover_shader;
        Shader* m_blurred_quad_shader;
        glm::vec4 m_hover_color;
        glm::vec4 m_selected_vertex_position = glm::vec4();
        glm::vec4 m_selected_edge_position = glm::vec4();
        float m_selected_edge_angle;

        VertexArrayObject* m_face_vao = nullptr;
        VertexArrayObject* m_edge_vao = nullptr;
        VertexArrayObject* m_vertex_vao = nullptr;
    };
}