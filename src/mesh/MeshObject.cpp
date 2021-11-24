#include "MeshObject.h"

#include <OpenVolumeMesh/Attribs/OpenVolumeMeshStatus.hh>
#include <OpenVolumeMesh/Attribs/NormalAttrib.hh>
#include <OpenVolumeMesh/Attribs/ColorAttrib.hh>
#include <OpenVolumeMesh/FileManager/FileManager.hh>
#include "../panels/LogWindow.h"
#include <array>
#include <string>
#include "../Window.h"

namespace vOS
{

    MeshObject::MeshObject()
    {
        m_mesh = new OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f>();
        m_should_update = false;


        OpenVolumeMesh::VertexPropertyT<bool> highlightProp = m_mesh->request_vertex_property<bool>("VertexHighlight");
        highlightProp->set_persistent(true);

        OpenVolumeMesh::VertexPropertyT<OpenVolumeMesh::Vec3f> highlightColProp = m_mesh->request_vertex_property<OpenVolumeMesh::Vec3f>(
                "VertexHighlightColor");
        highlightColProp->set_persistent(true);

    }

    void MeshObject::load_from_file(std::string file_path)
    {
        OpenVolumeMesh::IO::FileManager file_manager;
        file_manager.readFile(file_path, *m_mesh);

        remove_highlights();
        m_should_update = true;
    }

    void MeshObject::write_to_file(std::string file_path)
    {
        OpenVolumeMesh::IO::FileManager file_manager;
        file_manager.writeFile(file_path, *m_mesh);
    }

    void MeshObject::set_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> *mesh)
    {
        m_mesh = mesh;
        remove_highlights();
        m_should_update = true;
    }

    void MeshObject::update_vertex_buffer()
    {
        if (m_should_update)
        {
            if (m_vertexArrayObject != nullptr)
            {
                delete m_vertexArrayObject;
            }
            initialize_face_normals();
            initialize_vertex_normals();
            m_vertexArrayObject = new VertexArrayObject(vertices(), faces());
            m_vertexArrayObject->add_buffer(vertex_normals(), 1, 3);

            calculate_mesh_offset();
        }
        m_should_update = false;
    }

    void MeshObject::calculate_mesh_offset()
    {
        glm::vec3 min(m_vertices[0], m_vertices[1], m_vertices[2]);
        glm::vec3 max(m_vertices[0], m_vertices[1], m_vertices[2]);
        for (int i = 0; i < m_vertices.size(); i += 3)
        {
            glm::vec3 vertex(m_vertices[i], m_vertices[i + 1], m_vertices[i + 2]);
            if (vertex.x < min.x)
            {
                min.x = vertex.x;
            } else if (vertex.x > max.x)
            {
                max.x = vertex.x;
            }
            if (vertex.y < min.y)
            {
                min.y = vertex.y;
            } else if (vertex.y > max.y)
            {
                max.y = vertex.y;
            }
            if (vertex.z < min.z)
            {
                min.z = vertex.z;
            } else if (vertex.z > max.z)
            {
                max.z = vertex.z;
            }
        }
        m_mesh_offset_from_center = min + (max - min) * 0.5f;
    }

    std::vector<float> MeshObject::vertices()
    {
        //int dim = m_mesh->dim();
        int dim = 3;
        //std::vector<float> vertices (m_mesh->n_vertices() * dim);

        m_vertices.clear();
        m_vertices.reserve(m_mesh->n_vertices() * dim);

        for (OpenVolumeMesh::VertexIter v_it = m_mesh->vertices_begin();
             v_it != m_mesh->vertices_end(); ++v_it)
        {

            auto myPoint = m_mesh->vertex(*v_it);
            for (int i = 0; i < dim; i++)
            {
                m_vertices.push_back(myPoint[i]);
            }
        }

        return m_vertices;
    }

    std::vector<unsigned int> MeshObject::edges()
    {
        m_indices.clear();

        for (OpenVolumeMesh::EdgeIter e_it = m_mesh->edges_begin();
             e_it != m_mesh->edges_end(); ++e_it)
        {
            std::array<OpenVolumeMesh::VertexHandle, 2> edge_vertexids = m_mesh->edge_vertices(*e_it);
            m_indices.push_back(edge_vertexids[0].idx());
            m_indices.push_back(edge_vertexids[1].idx());
        }

        return m_indices;
    }


    std::vector<unsigned int> MeshObject::faces()
    {

        m_indices.clear();

        for (OpenVolumeMesh::FaceIter f_it = m_mesh->faces_begin();
             f_it != m_mesh->faces_end(); ++f_it)
        {
            for (auto halfface : m_mesh->face_halffaces(*f_it))
            {
                if (!m_mesh->is_boundary(halfface))
                {
                    continue;
                }
                auto face_vertexids = m_mesh->halfface_vertices(halfface);
                for (auto fv_it = face_vertexids.first;
                     fv_it != face_vertexids.second; ++fv_it)
                {
                    m_indices.push_back(fv_it->idx());
                }
            }
        }

        return m_indices;
    }

    std::vector<float> MeshObject::vertex_normals()
    {
        return m_vertex_normals;
    }

    std::vector<float> MeshObject::face_normals()
    {
        return m_face_normals;
    }

    void MeshObject::set_highlight(OpenVolumeMesh::VertexHandle v_h, bool b = true, float red = 0.0, float green = 0.0,
                                   float blue = 0.0, float alpha = 0.0)
    {
        //OpenVolumeMesh::VertexPropertyT<bool>  highlightProp = m_mesh->request_vertex_property<bool>("VertexHighlight");
        if (b == true)
        {
            std::tuple<OpenVolumeMesh::VertexHandle, float, float, float, float> tuple = std::make_tuple(v_h, red,
                                                                                                         green, blue,
                                                                                                         alpha);
            m_vertex_colors.push_back(tuple);
        } else if (b == false)
        {
            auto pos = std::find(m_vertex_colors.begin(), m_vertex_colors.end(),
                                 std::make_tuple(v_h, red, green, blue, alpha));
            if (pos != m_vertex_colors.end())
            {
                m_vertex_colors.erase(pos);
            }
        }

    }

    void MeshObject::remove_highlights()
    {
        m_vertex_colors.clear();
    }

    std::vector<std::tuple<OpenVolumeMesh::VertexHandle, float, float, float, float>> MeshObject::get_highlights()
    {
        return m_vertex_colors;
    }

    glm::vec3 &MeshObject::get_mesh_offset()
    {
        return m_mesh_offset_from_center;
    }

    void MeshObject::initialize_face_normals()
    {
        m_face_normals.clear();

        OpenVolumeMesh::NormalAttrib normals(*m_mesh);
        normals.update_face_normals();
        for (OpenVolumeMesh::FaceIter f_it = m_mesh->faces_begin();
             f_it != m_mesh->faces_end(); ++f_it)
        {
            for(int i = 0; i < normals[*f_it].size(); i++)
            {
                m_face_normals.push_back(normals[*f_it][i]);
            }
        }

    }

    void MeshObject::initialize_vertex_normals()
    {
        m_vertex_normals.clear();

        OpenVolumeMesh::NormalAttrib normals(*m_mesh);
        normals.update_vertex_normals();
        for (OpenVolumeMesh::VertexIter v_it = m_mesh->vertices_begin();
             v_it != m_mesh->vertices_end(); ++v_it)
        {
            float x = normals[*v_it][0];
            float y = normals[*v_it][1];
            float z = normals[*v_it][2];
            m_vertex_normals.push_back(x);
            m_vertex_normals.push_back(y);
            m_vertex_normals.push_back(z);

        }
    }

    VertexArrayObject* MeshObject::get_vao() const
    {
        return m_vertexArrayObject;
    }

}

