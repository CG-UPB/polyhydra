
#include "glad/glad.h"

#include "SelectionHoverPass.h"

#include "../meshes/CommonMeshes.h"
#include <cmath>
#include "../../Window.h"

namespace vOS
{
    SelectionHoverPass::SelectionHoverPass(): m_hover_color(glm::vec4(0.9, 0.2, 0.2, 0.5))
    {
        // Get shaders
        m_flat_color_shader = Shader::flat_color_shader();
        m_quad_circle_shader = Shader::quad_circle_shader();
        m_edge_hover_shader = Shader::edge_hover_shader();
        // Create VAOs
        m_quad_vao = new VertexArrayObject(CommonMeshes::PlaneXY::vertices(), CommonMeshes::PlaneXY::indices());
        m_quad_vao->add_attribute(CommonMeshes::PlaneXY::uvs(), 1, 2);
        m_edge_vao = new VertexArrayObject(CommonMeshes::Cylinder::vertices(), CommonMeshes::Cylinder::indices());
        // Default zoom point
        m_zoom_point = glm::vec3(0,0,0);
    }

    SelectionHoverPass::~SelectionHoverPass()
    {
        // Delete arrays
        delete m_face_vao;
        delete m_quad_vao;
    }

    void SelectionHoverPass::render(VertexArrayObject* vao, const RenderData& data, int mesh_id)
    {
        // Get Mesh
        MeshObject *obj = Window::instance().get_mesh_obj(mesh_id);
        if (obj == nullptr)
            return;

        // If no element is hovered, return
        if (m_hovered_type == SELECTION_TYPE_NONE || m_hovered_mesh != mesh_id)
        {
            return;
        }

        // GL Setup
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        // Transform Data
        glm::mat4 positionOffset = glm::translate(-obj->get_data().m_offset);
        glm::mat4 transform = data.camera.world * obj->get_data().get_transform() * positionOffset;

        if (m_hovered_type == SELECTION_TYPE_FACE)
        {
            // Face
            if (m_face_vao != nullptr)
            {
                // Draw flat color quad
                m_flat_color_shader->bind();
                m_flat_color_shader->set_uniform_mat4f("u_transform", transform);
                m_flat_color_shader->set_uniform_mat4f("u_projection", data.camera.projection);
                m_flat_color_shader->set_uniform_mat4f("u_view", data.camera.view);
                m_flat_color_shader->set_uniform_vec4f("u_color", m_hover_color);
                m_face_vao->draw();
                m_flat_color_shader->unbind();
            }
        }
        else if (m_hovered_type == SELECTION_TYPE_VERTEX)
        {
            // Vertex
            if (m_quad_vao != nullptr)
            {
                // Draw flat color sphere
                m_quad_circle_shader->bind();
                m_quad_circle_shader->set_uniform_mat4f("u_transform", transform);
                m_quad_circle_shader->set_uniform_mat4f("u_projection", data.camera.projection);
                m_quad_circle_shader->set_uniform_mat4f("u_view", data.camera.view);
                m_quad_circle_shader->set_uniform_vec4f("u_hover_color", m_hover_color);
                m_quad_circle_shader->set_uniform_vec4f("u_position", m_hovered_vertex_position);
                m_quad_circle_shader->set_uniform_float("u_scale", 0.005f);
                m_quad_vao->draw();
                m_quad_circle_shader->unbind();
            }
        }
        else if (m_hovered_type == SELECTION_TYPE_EDGE)
        {
            // Edge
            if (m_edge_vao != nullptr)
            {
                // Draw flat color cylinder
                glm::vec4 color(m_hover_color);
                color *= 0.5;
                m_edge_hover_shader->bind();
                m_edge_hover_shader->set_uniform_mat4f("u_mesh_transform", transform);
                m_edge_hover_shader->set_uniform_mat4f("u_projection", data.camera.projection);
                m_edge_hover_shader->set_uniform_mat4f("u_view", data.camera.view);
                m_edge_hover_shader->set_uniform_vec4f("u_color", color);
                m_edge_hover_shader->set_uniform_vec3f("u_from_vertex", m_hovered_edge_from);
                m_edge_hover_shader->set_uniform_vec3f("u_to_vertex", m_hovered_edge_to);
                m_edge_hover_shader->set_uniform_float("u_average_cell_size", obj->get_mvb()->get_average_cell_size());
                m_edge_vao->draw();
                m_edge_hover_shader->unbind();
            }
        }
    }

    void SelectionHoverPass::hover(const RenderData& data, int mesh_id, int type, int id)
    {

        // Get MeshObject
        MeshObject* mesh = Window::instance().get_mesh_obj(mesh_id);
        if(mesh == nullptr)
            return;

        // Do not doubly hover above an element
        if (m_hovered_id == id && m_hovered_type == type)
        {
            return;
        }

        // Remember Element data
        m_hovered_type = type;
        m_hovered_id = id;
        m_hovered_mesh = mesh_id;

        auto mesh_transform = mesh->get_data().get_transform();

        // Differentiate Actions depending on Element Type
        if (m_hovered_type == SELECTION_TYPE_FACE)
        {
            // Face Element
            auto mesh_data = get_face_mesh_data(*mesh, m_hovered_id);
            if (m_face_vao == nullptr)
            {
                // Create new VAO from Data
                m_face_vao = new VertexArrayObject(mesh_data.vertices, mesh_data.indices);
            }
            else
            {
                m_face_vao->update_vertices(mesh_data.vertices, mesh_data.indices);
            }
        }
        else if (m_hovered_type == SELECTION_TYPE_VERTEX)
        {
            // Vertex Element
            OpenVolumeMesh::VertexHandle vertex(m_hovered_id);
            if (vertex.is_valid())
            {
                // Get Vertex Position
                auto pos = mesh->m_mesh->vertex(vertex);
                m_hovered_vertex_position.x = pos[0];
                m_hovered_vertex_position.y = pos[1];
                m_hovered_vertex_position.z = pos[2];
                m_hovered_vertex_position.w = 1.0f;

                m_zoom_point = mesh_transform * m_hovered_vertex_position;
            }
        }
        else if (m_hovered_type == SELECTION_TYPE_EDGE)
        {
            // Edge Element
            OpenVolumeMesh::EdgeHandle edge(m_hovered_id);
            if (edge.is_valid())
            {
                // Get Edge Vertices
                auto edge_vertices = mesh->m_mesh->edge_vertices(edge);
                auto v0 = mesh->m_mesh->vertex(edge_vertices[0]);
                auto v1 = mesh->m_mesh->vertex(edge_vertices[1]);
                // Get Edge Vertex Positions
                m_hovered_edge_from.x = v0[0];
                m_hovered_edge_from.y = v0[1];
                m_hovered_edge_from.z = v0[2];
                m_hovered_edge_to.x = v1[0];
                m_hovered_edge_to.y = v1[1];
                m_hovered_edge_to.z = v1[2];

                m_zoom_point = glm::vec3(mesh_transform * glm::vec4((m_hovered_edge_from + m_hovered_edge_to) / 2.0f, 1.0f));
            }
        }
    }

    SelectionHoverPass::HoverMeshData SelectionHoverPass::get_face_mesh_data(MeshObject& mesh, int face_id)
    {
        HoverMeshData res;
        // Get Face from OVM
        OpenVolumeMesh::FaceHandle face(face_id);
        if (face.is_valid())
        {
            OpenVolumeMesh::VectorT<double,3> midpoint = OpenVolumeMesh::VectorT<double,3>(0,0,0);
            // Get all Vertices and remember amount of vertices
            int num_vertices = 0;
            for (auto v_h : mesh.m_mesh->face_vertices(face))
            {
                auto vertex = mesh.m_mesh->vertex(v_h);
                res.vertices.push_back(vertex[0]);
                res.vertices.push_back(vertex[1]);
                res.vertices.push_back(vertex[2]);
                midpoint += vertex;
                num_vertices++;
            }


            // Add a new Midpoint Vertex, number of vertices exceed 4
            if(num_vertices > 4){
                midpoint /= num_vertices;

                res.vertices.push_back(midpoint[0]);
                res.vertices.push_back(midpoint[1]);
                res.vertices.push_back(midpoint[2]);
            }

            // Triangulate Face
            switch (num_vertices)
            {
                case 3:
                {
                    // simplest case, just connect the three vertices to a triangle
                    res.indices.push_back(num_vertices + 0);
                    res.indices.push_back(num_vertices + 2);
                    res.indices.push_back(num_vertices + 1);
                    break;
                }
                case 4:
                {
                    // we have 4 vertices, so we need to create two triangles out of it
                    res.indices.push_back(num_vertices + 0);
                    res.indices.push_back(num_vertices + 2);
                    res.indices.push_back(num_vertices + 1);

                    res.indices.push_back(num_vertices + 0);
                    res.indices.push_back(num_vertices + 3);
                    res.indices.push_back(num_vertices + 2);
                    break;
                }
                default:
                {

                    // Triangulate in such a way, that every triangle uses the midpoint (with id 0) is part of the triangle
                    for(int i = 0; i< num_vertices - 2; i++){
                        res.indices.push_back(num_vertices + num_vertices - 1);
                        res.indices.push_back(num_vertices + i + 1);
                        res.indices.push_back(num_vertices + i + 2);
                    }

                    // The Last Triangle Vertex IDs loop back around
                    res.indices.push_back(num_vertices + num_vertices - 1);
                    res.indices.push_back(num_vertices);
                    res.indices.push_back(num_vertices + 1);
                    break;
                }
            }

            // Set other mesh data
            auto mesh_transform = mesh.get_data().get_transform();
            auto center = mesh.m_mesh->barycenter(face);
            auto pos = mesh_transform * glm::vec4(center[0], center[1], center[2], 1.0f);
            m_zoom_point = glm::vec3(pos);

        }
        return res;
    }

    /*
    SelectionHoverPass::HoverMeshData SelectionHoverPass::get_edge_mesh_data(MeshObject& mesh, const RenderData& data, int mesh_id, int edge_id)
    {
        HoverMeshData res;

        // Get Mesh
        MeshObject *obj = Window::instance().get_mesh_obj(mesh_id);
        if (obj == nullptr)
            return res;

        float width = 0.003;

        OpenVolumeMesh::EdgeHandle edge(edge_id);
        auto edge_vertices = mesh.m_mesh->edge_vertices(edge);
        auto v0 = mesh.m_mesh->vertex(edge_vertices[0]);
        auto v1 = mesh.m_mesh->vertex(edge_vertices[1]);
        glm::vec3 pos0 = glm::vec3(v0[0], v0[1], v0[2]);
        glm::vec3 pos1 = glm::vec3(v1[0], v1[1], v1[2]);
        auto mat = data.camera.projection * data.camera.view * data.camera.world * obj->get_data().get_transform();
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
     */
}