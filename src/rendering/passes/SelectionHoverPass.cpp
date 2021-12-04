
#include "glad/glad.h"

#include "SelectionHoverPass.h"

#include "../meshes/CommonMeshes.h"
#include <cmath>

namespace vOS
{
    SelectionHoverPass::SelectionHoverPass(): m_hover_color(glm::vec4(0.9, 0.9, 0.2, 0.8))
    {
        m_hover_shader = Shader::selection_hover_shader();
        m_blurred_quad_shader = Shader::blurred_quad_shader();
        m_vertex_vao = new VertexArrayObject(CommonMeshes::PlaneXY::vertices(), CommonMeshes::PlaneXY::indices());
        m_vertex_vao->add_attribute(CommonMeshes::PlaneXY::uvs(), 1, 2);
    }

    SelectionHoverPass::~SelectionHoverPass()
    {
        delete m_face_vao;
        delete m_vertex_vao;
        delete m_edge_vao;
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
                m_hover_shader->bind();
                m_hover_shader->set_uniform_mat4f("u_mesh_transform", transform);
                m_hover_shader->set_uniform_mat4f("u_projection", data.camera.projection);
                m_hover_shader->set_uniform_mat4f("u_view", data.camera.view);
                m_hover_shader->set_uniform_vec4f("u_hover_color", m_hover_color);
                m_face_vao->draw();
                m_hover_shader->unbind();
            }
        }
        else if (m_selected_type == SELECTION_TYPE_VERTEX)
        {
            if (m_vertex_vao != nullptr)
            {
                m_blurred_quad_shader->bind();
                m_blurred_quad_shader->set_uniform_mat4f("u_transform", transform);
                m_blurred_quad_shader->set_uniform_mat4f("u_projection", data.camera.projection);
                m_blurred_quad_shader->set_uniform_mat4f("u_view", data.camera.view);
                m_blurred_quad_shader->set_uniform_vec4f("u_hover_color", m_hover_color);
                m_blurred_quad_shader->set_uniform_vec4f("u_position", m_selected_vertex_position);
                m_blurred_quad_shader->set_uniform_float("u_scale", 0.025f);
                m_blurred_quad_shader->set_uniform_float("u_rotation", 0.0f);
                m_vertex_vao->draw();
                m_blurred_quad_shader->unbind();
            }
        }
        else if (m_selected_type == SELECTION_TYPE_EDGE)
        {
            if (m_edge_vao != nullptr)
            {
                m_blurred_quad_shader->bind();
                m_blurred_quad_shader->set_uniform_mat4f("u_transform", transform);
                m_blurred_quad_shader->set_uniform_mat4f("u_projection", data.camera.projection);
                m_blurred_quad_shader->set_uniform_mat4f("u_view", data.camera.view);
                m_blurred_quad_shader->set_uniform_vec4f("u_hover_color", m_hover_color);
                m_blurred_quad_shader->set_uniform_vec4f("u_position", m_selected_edge_position);
                m_blurred_quad_shader->set_uniform_float("u_scale", 0.025f);
                m_blurred_quad_shader->set_uniform_float("u_rotation", m_selected_edge_angle);
                m_edge_vao->draw();
                m_blurred_quad_shader->unbind();
            }
        }
    }

    void SelectionHoverPass::select(MeshObject& mesh, int type, int id)
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
                glm::vec3 pos0 = glm::vec3(v0[0], v0[1], v0[2]);
                glm::vec3 pos1 = glm::vec3(v1[0], v1[1], v1[2]);

                float dot = pos0.x * pos1.x + pos0.y * pos1.y;
                float det = pos0.x * pos1.x - pos0.y * pos1.y;
                m_selected_edge_angle = std::atan2(det, dot);

                glm::vec3 edge_normal = glm::normalize(glm::cross(pos1 - pos0, glm::vec3(0.0, 0.0, 1.0)));
                //m_selected_edge_angle = glm::acos(glm::dot(glm::vec3(0.0f, 1.0f, 0.0f), edge_normal));
                std::cout << "angle: " << glm::degrees(m_selected_edge_angle) << std::endl;
                m_selected_edge_position.x = pos0.x + (pos1.x - pos0.x) * 0.5f;
                m_selected_edge_position.y = pos0.y + (pos1.y - pos0.y) * 0.5f;
                m_selected_edge_position.z = pos0.z + (pos1.z - pos0.z) * 0.5f;
                m_selected_edge_position.w = 1.0f;
                float width = glm::length(pos1 - pos0);
                if (m_edge_vao == nullptr)
                {
                    m_edge_vao = new VertexArrayObject(CommonMeshes::PlaneXY::vertices(2.0f, 1.0f), CommonMeshes::PlaneXY::indices());
                    m_edge_vao->add_attribute(CommonMeshes::PlaneXY::uvs(), 1, 2);
                }
                else
                {
                    m_edge_vao->update_vertices(CommonMeshes::PlaneXY::vertices(2.0f, 1.0f), CommonMeshes::PlaneXY::indices());
                }
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
        }
        return res;
    }

    SelectionHoverPass::MeshData SelectionHoverPass::get_edge_mesh_data(MeshObject& mesh, int edge_id)
    {
        MeshData res;

        float width = 0.3;

        OpenVolumeMesh::EdgeHandle edge(edge_id);
        auto edge_vertices = mesh.m_mesh->edge_vertices(edge);
        auto v0 = mesh.m_mesh->vertex(edge_vertices[0]);
        auto v1 = mesh.m_mesh->vertex(edge_vertices[1]);
        glm::vec3 pos0 = glm::vec3(v0[0], v0[1], v0[2]);
        glm::vec3 pos1 = glm::vec3(v1[0], v1[1], v1[2]);
        glm::vec3 edge_normal = glm::normalize(glm::cross(pos1 - pos0, glm::vec3(0.0, 0.0, 1.0)));

        add_vertex(pos0 - width * edge_normal, res.vertices);
        add_vertex(pos1 - width * edge_normal, res.vertices);
        add_vertex(pos0 + width * edge_normal, res.vertices);
        add_vertex(pos1 + width * edge_normal, res.vertices);

        res.indices.push_back(0);
        res.indices.push_back(1);
        res.indices.push_back(2);

        res.indices.push_back(0);
        res.indices.push_back(2);
        res.indices.push_back(3);

        return res;
    }

    void SelectionHoverPass::add_vertex(const glm::vec3& vertex, std::vector<float>& vertices)
    {
        vertices.push_back(vertex.x);
        vertices.push_back(vertex.y);
        vertices.push_back(vertex.z);
    }
}