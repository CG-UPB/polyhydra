#include "MeshObject.h"

#include <OpenVolumeMesh/Attribs/OpenVolumeMeshStatus.hh>
#include <OpenVolumeMesh/Attribs/NormalAttrib.hh>
#include <OpenVolumeMesh/Attribs/ColorAttrib.hh>
#include <OpenVolumeMesh/FileManager/FileManager.hh>
#include "../panels/LogWindow.h"
#include <array>
#include <string>
#include "../Window.h"
#include "../rendering/meshes/CommonMeshes.h"

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

    MeshObject::MeshObject(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> *mesh) : MeshObject()
    {
        set_mesh(mesh);
    }

    void MeshObject::load_from_file(std::string file_path)
    {
        OpenVolumeMesh::IO::FileManager file_manager;
        file_manager.readFile(file_path, *m_mesh);

        remove_highlights();
        m_should_update = true;
    }

    void MeshObject::write_to_file(const std::string& file_path) const
    {
        OpenVolumeMesh::IO::FileManager file_manager;
        file_manager.writeFile(file_path, *m_mesh);
    }

    void MeshObject::set_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> *mesh)
    {
        m_mesh = new OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f>();
        m_mesh->assign(mesh);

        init_vertices();
        init_edges();
        init_faces();
        //init_cells();
        init_vertex_normals();
        init_face_normals();

        remove_highlights();
        m_should_update = true;
    }

    void MeshObject::update_vertex_buffer()
    {
        if (m_should_update)
        {
            delete m_vertexArrayObject;
            m_vertexArrayObject = new VertexArrayObject(vertices(), faces());
            m_vertexArrayObject->add_attribute(vertex_normals(), 1, 3);

            delete m_sphere_vao;
            // add an attribute for each vertex position, so we can render the spheres instanced
            m_sphere_vao = new VertexArrayObject(CommonMeshes::Sphere::vertices(), CommonMeshes::Sphere::indices());
            m_sphere_vao->add_attribute(m_vertex_normals, 1, 3, true);
            m_sphere_vao->add_attribute(m_vertices, 2, 3, true);

            delete m_cylinder_vao;
            m_cylinder_vao = new VertexArrayObject(CommonMeshes::Cylinder::vertices(), CommonMeshes::Cylinder::indices());
            // collect from-vertex and to-vertex for each edge
            std::vector<float> from_vertices;
            std::vector<float> to_vertices;
            m_edge_ids.clear();
            for (int i = 0; i < m_indices.size(); i += 3)
            {
                auto vh0 = OpenVolumeMesh::VertexHandle( m_indices[i + 0]);
                auto vh1 = OpenVolumeMesh::VertexHandle( m_indices[i + 1]);
                auto vh2 = OpenVolumeMesh::VertexHandle( m_indices[i + 2]);

                for (auto heh : m_mesh->outgoing_halfedges(vh0))
                {
                    auto out = m_mesh->to_vertex_handle(heh);
                    if (out == vh1)
                    {
                        m_edge_ids.push_back(m_mesh->edge_handle(heh).idx());
                        break;
                    }
                }
                for (auto heh : m_mesh->outgoing_halfedges(vh1))
                {
                    auto out = m_mesh->to_vertex_handle(heh);
                    if (out == vh2)
                    {
                        m_edge_ids.push_back(m_mesh->edge_handle(heh).idx());
                        break;
                    }
                }
                for (auto heh : m_mesh->outgoing_halfedges(vh2))
                {
                    auto out = m_mesh->to_vertex_handle(heh);
                    if (out == vh0)
                    {
                        m_edge_ids.push_back(m_mesh->edge_handle(heh).idx());
                        break;
                    }
                }

                auto v0 = m_mesh->vertex(vh0);
                auto v1 = m_mesh->vertex(vh1);
                auto v2 = m_mesh->vertex(vh2);

                from_vertices.push_back(v0[0]);
                from_vertices.push_back(v0[1]);
                from_vertices.push_back(v0[2]);

                to_vertices.push_back(v1[0]);
                to_vertices.push_back(v1[1]);
                to_vertices.push_back(v1[2]);

                from_vertices.push_back(v1[0]);
                from_vertices.push_back(v1[1]);
                from_vertices.push_back(v1[2]);

                to_vertices.push_back(v2[0]);
                to_vertices.push_back(v2[1]);
                to_vertices.push_back(v2[2]);

                from_vertices.push_back(v2[0]);
                from_vertices.push_back(v2[1]);
                from_vertices.push_back(v2[2]);

                to_vertices.push_back(v0[0]);
                to_vertices.push_back(v0[1]);
                to_vertices.push_back(v0[2]);
            }
            m_cylinder_vao->add_attribute(from_vertices, 1, 3, true);
            m_cylinder_vao->add_attribute(to_vertices, 2, 3, true);

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

    void MeshObject::init_vertices()
    {
        int dim = 3;

        m_vertices.clear();
        m_vertices.reserve(m_mesh->n_vertices() * dim);

        for (OpenVolumeMesh::VertexIter v_it = m_mesh->vertices_begin();
             v_it != m_mesh->vertices_end(); ++v_it)
        {

            auto vertex_coords = m_mesh->vertex(*v_it);
            for (int i = 0; i < dim; i++)
            {
                m_vertices.push_back(vertex_coords[i]);
            }
        }
    }

    void MeshObject::init_edges()
    {
        m_faces.clear();

        for (OpenVolumeMesh::EdgeIter e_it = m_mesh->edges_begin();
             e_it != m_mesh->edges_end(); ++e_it)
        {
            std::array<OpenVolumeMesh::VertexHandle, 2> edge_vertexids = m_mesh->edge_vertices(*e_it);
            m_faces.push_back(edge_vertexids[0].idx());
            m_faces.push_back(edge_vertexids[1].idx());
        }

    }


    void MeshObject::init_faces()
    {

        m_faces.clear();

        int count = 0;
        std::vector<int> vert_idx;

        for (OpenVolumeMesh::FaceIter f_it = m_mesh->faces_begin();
             f_it != m_mesh->faces_end(); ++f_it)
        {

            for (auto halfface : m_mesh->face_halffaces(*f_it))
            {
                if (!m_mesh->is_boundary(halfface))
                {
                    continue;
                }
                auto face_vertex_ids = m_mesh->halfface_vertices(halfface);

                count = 0;
                vert_idx.clear();

                //count how many vertices the face has
                for (auto fv_it = face_vertex_ids.first;
                     fv_it != face_vertex_ids.second; ++fv_it)
                {
                    count ++;
                    vert_idx.push_back(fv_it->idx());
                }

                //save indices depending on count
                if (count == 3)
                {
                    // create 1 triangles out of 3 indices
                    m_faces.push_back(vert_idx[0]);
                    m_faces.push_back(vert_idx[1]);
                    m_faces.push_back(vert_idx[2]);
                }
                else if (count == 4)
                {
                    // create 2 triangles out of 4 indices
                    m_faces.push_back(vert_idx[0]);
                    m_faces.push_back(vert_idx[1]);
                    m_faces.push_back(vert_idx[2]);

                    m_faces.push_back(vert_idx[0]);
                    m_faces.push_back(vert_idx[2]);
                    m_faces.push_back(vert_idx[3]);



                }
                // unpredictable behaviour
                else
                {
                    for(int i = 0; i < count; i++)
                    {
                        m_faces.push_back(vert_idx[i]);
                    }
                }

                m_face_ids.push_back(f_it->idx());


            }
        }
    }

    unsigned int MeshObject::to_faceID(unsigned int value)
    {
        if(m_face_ids.size() > value)
        {
            return m_face_ids[value] + 1;
        }
        return 0;
    }

    unsigned int MeshObject::to_edgeID(int value)
    {
        if(m_edge_ids.size() > value)
        {
            return m_edge_ids[value] + 1;
        }
        return 0;
    }

    void MeshObject::set_highlight(OpenVolumeMesh::VertexHandle v_h, bool b = true, float red = 0.0, float green = 0.0,
                                   float blue = 0.0, float alpha = 0.0)
    {
        //OpenVolumeMesh::VertexPropertyT<bool>  highlightProp = m_mesh->request_vertex_property<bool>("VertexHighlight");
        if (b)
        {
            std::tuple<OpenVolumeMesh::VertexHandle, float, float, float, float> tuple = std::make_tuple(v_h, red,
                                                                                                         green, blue,
                                                                                                         alpha);
            m_vertex_colors.push_back(tuple);
        } else {
            auto pos = std::find(m_vertex_colors.begin(), m_vertex_colors.end(),
                                 std::make_tuple(v_h, red, green, blue, alpha));
            if (pos != m_vertex_colors.end()) {
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

    void MeshObject::init_face_normals()
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

    void MeshObject::init_vertex_normals()
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

    VertexArrayObject* MeshObject::get_sphere_vao() const
    {
        return m_sphere_vao;
    }
    int MeshObject::calculate_selection_size()
    {
        return m_face_ids.size();
    }

    int MeshObject::get_num_visible_vertices() const
    {
        return (int) m_vertices.size() / 3;
    }

    VertexArrayObject* MeshObject::get_cylinder_vao() const
    {
        return m_cylinder_vao;
    }

    int MeshObject::get_num_visible_edges() const
    {
        return (int) m_indices.size();
    }

    void MeshObject::set_selection_offset(int start)
    {
        m_selection_offset = {start, start + calculate_selection_size() -1};
    }
}

