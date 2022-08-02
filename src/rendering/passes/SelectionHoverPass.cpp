
#include "SelectionHoverPass.h"
#include "../meshes/CommonMeshes.h"
#include "../gl/Shader.h"
#include "../Renderer.h"

namespace volumeshOS::Internal
{
    SelectionHoverPass::SelectionHoverPass(): m_hover_color(glm::vec4(0.9, 0.2, 0.2, 0.5))
    {
        // Get shaders
        m_flat_color_shader = Shader::flat_color_shader();
        m_quad_circle_shader = Shader::quad_circle_shader();
        m_edge_hover_shader = Shader::edge_hover_shader();
        // Create VAOs
        m_quad_vao = std::make_unique<VertexArrayObject>(CommonMeshes::PlaneXY::vertices(), CommonMeshes::PlaneXY::indices());
        m_quad_vao->add_attribute(CommonMeshes::PlaneXY::uvs(), 1, 2);
        m_edge_vao = std::make_unique<VertexArrayObject>(CommonMeshes::Cylinder::vertices(), CommonMeshes::Cylinder::indices());
    }

    void SelectionHoverPass::render(const Renderer& renderer)
    {
        if (m_hovered_type == SELECTION_TYPE_NONE)
        {
            return;
        }

        // GL Setup
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_FRAMEBUFFER_SRGB);

        renderer.buffers.target_framebuffer_ms->bind();

        for (const auto& mesh : renderer.render_list)
        {
            // If no element is hovered, return
            if (m_hovered_mesh != mesh->get_id())
            {
                continue;
            }

            // Transform Data
            glm::mat4 transform = renderer.camera->world * mesh->get_data().get_transform();

            if (m_hovered_type == SELECTION_TYPE_VERTEX)
            {
                // Vertex
                if (m_quad_vao != nullptr)
                {
                    // Draw flat color sphere
                    m_quad_circle_shader->bind();
                    m_quad_circle_shader->set_uniform_mat4f("u_transform", transform);
                    m_quad_circle_shader->set_uniform_mat4f("u_projection", renderer.camera->projection);
                    m_quad_circle_shader->set_uniform_mat4f("u_view", renderer.camera->view);
                    m_quad_circle_shader->set_uniform_vec4f("u_hover_color", m_hover_color);
                    m_quad_circle_shader->set_uniform_vec4f("u_position", m_hovered_vertex_position);
                    m_quad_circle_shader->set_uniform_float("u_scale", 0.15f);
                    m_quad_circle_shader->set_uniform_float("u_average_cell_size", mesh->get_mvb()->get_average_cell_size());
                    m_quad_circle_shader->set_uniform_float("u_gamma", AppState::settings.gamma);
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
                    m_edge_hover_shader->set_uniform_mat4f("u_projection", renderer.camera->projection);
                    m_edge_hover_shader->set_uniform_mat4f("u_view", renderer.camera->view);
                    m_edge_hover_shader->set_uniform_vec4f("u_color", color);
                    m_edge_hover_shader->set_uniform_vec3f("u_from_vertex", m_hovered_edge_from);
                    m_edge_hover_shader->set_uniform_vec3f("u_to_vertex", m_hovered_edge_to);
                    m_edge_hover_shader->set_uniform_float("u_cell_size", mesh->get_data().cell_size);
                    m_edge_hover_shader->set_uniform_float("u_average_cell_size", mesh->get_mvb()->get_average_cell_size());
                    m_edge_hover_shader->set_uniform_float("u_gamma", AppState::settings.gamma);
                    m_edge_vao->draw();
                    m_edge_hover_shader->unbind();
                }
            }
        }
        renderer.buffers.target_framebuffer_ms->unbind();

        glDisable(GL_FRAMEBUFFER_SRGB);
    }

    void SelectionHoverPass::hover(const std::shared_ptr<MeshObject>& mesh, int type, int id)
    {
        if(mesh != nullptr && type == SELECTION_TYPE_NONE)
        {
            m_hovered_mesh = mesh->get_id();
        }

        // Do not doubly hover above an element
        if (m_hovered_id == id && m_hovered_type == type)
        {
            return;
        }

        // Remember Element data
        m_hovered_type = type;
        m_hovered_id = id;

        if (mesh == nullptr)
        {
            return;
        }

        auto mesh_transform = mesh->get_data().get_transform();

        // Differentiate Actions depending on Element Type
        if (m_hovered_type == SELECTION_TYPE_FACE)
        {
            // Face Element
            auto mesh_data = get_face_mesh_data(*mesh, m_hovered_id);
            if (m_face_vao == nullptr)
            {
                // Create new VAO from Data
                m_face_vao = std::make_unique<VertexArrayObject>(mesh_data.vertices, mesh_data.indices);
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
                auto pos = mesh->get_ovm()->vertex(vertex);
                m_hovered_vertex_position.x = pos[0];
                m_hovered_vertex_position.y = pos[1];
                m_hovered_vertex_position.z = pos[2];
                m_hovered_vertex_position.w = 1.0f;
            }
        }
        else if (m_hovered_type == SELECTION_TYPE_EDGE)
        {
            // Edge Element
            OpenVolumeMesh::EdgeHandle edge(m_hovered_id);
            if (edge.is_valid())
            {
                // Get Edge Vertices
                auto edge_vertices = mesh->get_ovm()->edge_vertices(edge);
                auto v0 = mesh->get_ovm()->vertex(edge_vertices[0]);
                auto v1 = mesh->get_ovm()->vertex(edge_vertices[1]);
                // Get Edge Vertex Positions
                m_hovered_edge_from.x = v0[0];
                m_hovered_edge_from.y = v0[1];
                m_hovered_edge_from.z = v0[2];
                m_hovered_edge_to.x = v1[0];
                m_hovered_edge_to.y = v1[1];
                m_hovered_edge_to.z = v1[2];

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
            for (auto v_h : mesh.get_ovm()->face_vertices(face))
            {
                auto vertex = mesh.get_ovm()->vertex(v_h);
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
            auto center = mesh.get_ovm()->barycenter(face);
            auto pos = mesh_transform * glm::vec4(center[0], center[1], center[2], 1.0f);
        }
        return res;
    }
}