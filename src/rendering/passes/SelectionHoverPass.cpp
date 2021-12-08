
#include "glad/glad.h"

#include "SelectionHoverPass.h"

#include "../meshes/CommonMeshes.h"
#include <cmath>

namespace vOS
{
    SelectionHoverPass::SelectionHoverPass(): m_hover_color(glm::vec4(0.9, 0.2, 0.2, 0.5))
    {
        m_flat_color_shader = Shader::flat_color_shader();
        m_quad_circle_shader = Shader::quad_circle_shader();
        m_edge_hover_shader = Shader::edge_hover_shader();
        m_quad_vao = new VertexArrayObject(CommonMeshes::PlaneXY::vertices(), CommonMeshes::PlaneXY::indices());
        m_quad_vao->add_attribute(CommonMeshes::PlaneXY::uvs(), 1, 2);
        m_edge_vao = new VertexArrayObject(CommonMeshes::Cylinder::vertices(), CommonMeshes::Cylinder::indices());
        m_zoom_point = glm::vec3(0,0,0);
    }

    SelectionHoverPass::~SelectionHoverPass()
    {
        delete m_face_vao;
        delete m_quad_vao;
    }

    void SelectionHoverPass::render(VertexArrayObject* vao, const RenderData& data)
    {
        if (m_selected_type == SELECTION_TYPE_NONE)
        {
            return;
        }

        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        glm::mat4 positionOffset = glm::translate(-data.mesh.offset);
        glm::mat4 transform = data.camera.world * data.mesh.transform * positionOffset;

        if (m_selected_type == SELECTION_TYPE_FACE)
        {
            if (m_face_vao != nullptr)
            {
                m_flat_color_shader->bind();
                m_flat_color_shader->set_uniform_mat4f("u_transform", transform);
                m_flat_color_shader->set_uniform_mat4f("u_projection", data.camera.projection);
                m_flat_color_shader->set_uniform_mat4f("u_view", data.camera.view);
                m_flat_color_shader->set_uniform_vec4f("u_color", m_hover_color);
                m_face_vao->draw();
                m_flat_color_shader->unbind();
            }
        }
        else if (m_selected_type == SELECTION_TYPE_VERTEX)
        {
            if (m_quad_vao != nullptr)
            {
                m_quad_circle_shader->bind();
                m_quad_circle_shader->set_uniform_mat4f("u_transform", transform);
                m_quad_circle_shader->set_uniform_mat4f("u_projection", data.camera.projection);
                m_quad_circle_shader->set_uniform_mat4f("u_view", data.camera.view);
                m_quad_circle_shader->set_uniform_vec4f("u_hover_color", m_hover_color);
                m_quad_circle_shader->set_uniform_vec4f("u_position", m_selected_vertex_position);
                m_quad_circle_shader->set_uniform_float("u_scale", 0.015f);
                m_quad_vao->draw();
                m_quad_circle_shader->unbind();
            }
        }
        else if (m_selected_type == SELECTION_TYPE_EDGE)
        {
            if (m_edge_vao != nullptr)
            {
                glm::vec4 color(m_hover_color);
                color *= 0.5;
                m_edge_hover_shader->bind();
                m_edge_hover_shader->set_uniform_mat4f("u_mesh_transform", transform);
                m_edge_hover_shader->set_uniform_mat4f("u_projection", data.camera.projection);
                m_edge_hover_shader->set_uniform_mat4f("u_view", data.camera.view);
                m_edge_hover_shader->set_uniform_vec4f("u_color", color);
                m_edge_hover_shader->set_uniform_vec3f("u_from_vertex", m_selected_edge_from);
                m_edge_hover_shader->set_uniform_vec3f("u_to_vertex", m_selected_edge_to);
                m_edge_vao->draw();
                m_edge_hover_shader->unbind();
            }
        }
    }

    void SelectionHoverPass::select(MeshObject& mesh, const RenderData& data, int type, int id)
    {
        if (m_selected_id == id && m_selected_type == type)
        {
            return;
        }
        m_selected_type = type;
        m_selected_id = id;

        if (m_selected_type == SELECTION_TYPE_FACE)
        {
            auto mesh_data = get_face_mesh_data(mesh, m_selected_id);
            if (m_face_vao == nullptr)
            {
                m_face_vao = new VertexArrayObject(mesh_data.vertices, mesh_data.indices);
            }
            else
            {
                m_face_vao->update_vertices(mesh_data.vertices, mesh_data.indices);
            }
        }
        else if (m_selected_type == SELECTION_TYPE_VERTEX)
        {
            OpenVolumeMesh::VertexHandle vertex(m_selected_id);
            if (vertex.is_valid())
            {
                auto pos = mesh.m_mesh->vertex(vertex);
                m_selected_vertex_position.x = pos[0];
                m_selected_vertex_position.y = pos[1];
                m_selected_vertex_position.z = pos[2];
                m_selected_vertex_position.w = 1.0f;

                m_zoom_point.x = m_selected_vertex_position.x;
                m_zoom_point.y = m_selected_vertex_position.y;
                m_zoom_point.z = m_selected_vertex_position.z;
            }
        }
        else if (m_selected_type == SELECTION_TYPE_EDGE)
        {
            OpenVolumeMesh::EdgeHandle edge(m_selected_id);
            if (edge.is_valid())
            {
                auto edge_vertices = mesh.m_mesh->edge_vertices(edge);
                auto v0 = mesh.m_mesh->vertex(edge_vertices[0]);
                auto v1 = mesh.m_mesh->vertex(edge_vertices[1]);
                m_selected_edge_from.x = v0[0];
                m_selected_edge_from.y = v0[1];
                m_selected_edge_from.z = v0[2];
                m_selected_edge_to.x = v1[0];
                m_selected_edge_to.y = v1[1];
                m_selected_edge_to.z = v1[2];

                m_zoom_point.x = (m_selected_edge_from.x + m_selected_edge_to.x) / 2;
                m_zoom_point.y = (m_selected_edge_from.y + m_selected_edge_to.y) / 2;
                m_zoom_point.z = (m_selected_edge_from.z + m_selected_edge_to.z) / 2;
            }
        }
    }

    SelectionHoverPass::MeshData SelectionHoverPass::get_face_mesh_data(MeshObject& mesh, int face_id)
    {
        MeshData res;

        OpenVolumeMesh::FaceHandle face(face_id);
        if (face.is_valid())
        {
            int index = 0;
            for (auto v_h : mesh.m_mesh->face_vertices(face))
            {
                auto vertex = mesh.m_mesh->vertex(v_h);
                res.vertices.push_back(vertex[0]);
                res.vertices.push_back(vertex[1]);
                res.vertices.push_back(vertex[2]);

                res.indices.push_back(index++);

            }

            auto pos = mesh.m_mesh->barycenter(face);
            m_zoom_point.x = pos[0];
            m_zoom_point.y = pos[1];
            m_zoom_point.z = pos[2];

        }
        return res;
    }

    SelectionHoverPass::MeshData SelectionHoverPass::get_edge_mesh_data(MeshObject& mesh, const RenderData& data, int edge_id)
    {
        MeshData res;

        float width = 0.003;

        OpenVolumeMesh::EdgeHandle edge(edge_id);
        auto edge_vertices = mesh.m_mesh->edge_vertices(edge);
        auto v0 = mesh.m_mesh->vertex(edge_vertices[0]);
        auto v1 = mesh.m_mesh->vertex(edge_vertices[1]);
        glm::vec3 pos0 = glm::vec3(v0[0], v0[1], v0[2]);
        glm::vec3 pos1 = glm::vec3(v1[0], v1[1], v1[2]);
        auto mat = data.camera.projection * data.camera.view * data.camera.world * data.mesh.transform;
        auto p0_transformed = mat * glm::vec4(pos0, 1.0f);
        auto p1_transformed = mat * glm::vec4(pos1, 1.0f);
        glm::vec3 edge_normal = glm::normalize(glm::cross(glm::vec3(p1_transformed - p0_transformed), glm::vec3(0.0, 0.0, 1.0)));

        add_vertex(pos0 - width * edge_normal, res.vertices);
        add_vertex(pos1 - width * edge_normal, res.vertices);
        add_vertex(pos0 + width * edge_normal, res.vertices);
        add_vertex(pos1 + width * edge_normal, res.vertices);

        res.indices.push_back(0);
        res.indices.push_back(1);
        res.indices.push_back(2);

        res.indices.push_back(2);
        res.indices.push_back(3);
        res.indices.push_back(0);

        return res;
    }

    void SelectionHoverPass::add_vertex(const glm::vec3& vertex, std::vector<float>& vertices)
    {
        vertices.push_back(vertex.x);
        vertices.push_back(vertex.y);
        vertices.push_back(vertex.z);
    }
}