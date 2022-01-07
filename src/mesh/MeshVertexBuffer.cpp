
#include "MeshVertexBuffer.h"
#include "../rendering/meshes/CommonMeshes.h"

#include <OpenVolumeMesh/Attribs/NormalAttrib.hh>

namespace vOS
{

    MeshVertexBuffer::MeshVertexBuffer(Mesh* mesh, BufferSpecification spec): m_spec(spec)
    {
        int depth = spec.peel_depth;

        Mesh* current_mesh = mesh;
        m_original_vertices = get_vertices(*current_mesh);
        if (depth > 0)
        {
            current_mesh = new OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f>();
            current_mesh->enable_deferred_deletion(true);
            current_mesh->assign(mesh);

            for (int i = 0; i < depth; i++)
            {
                delete_boundary_cells(*current_mesh);
            }
        }

        generate_buffer(*current_mesh);

        m_vao = new VertexArrayObject(m_positions, m_indices);
        m_vao->add_attribute(m_normals, 1, 3);
        m_vao->add_attribute(m_cell_centers, 2, 3);

        m_sphere_vao = new VertexArrayObject(CommonMeshes::Sphere::selection_sphere().vertices(),
                                             CommonMeshes::Sphere::selection_sphere().indices());

        m_sphere_vao->add_attribute(m_normals, 1, 3, true);
        m_sphere_vao->add_attribute(m_positions, 2, 3, true);
        m_sphere_vao->add_attribute(m_cell_centers, 3, 3, true);

        m_cylinder_vao = new VertexArrayObject(CommonMeshes::Cylinder::edge_cylinder().vertices(),
                                               CommonMeshes::Cylinder::edge_cylinder().indices());

        m_cylinder_vao->add_attribute(m_from_vertices, 1, 3, true);
        m_cylinder_vao->add_attribute(m_to_vertices, 2, 3, true);
        m_cylinder_vao->add_attribute(m_cell_centers, 3, 3, true);

        if (depth > 0)
        {
            delete current_mesh;
        }
    }

    MeshVertexBuffer::~MeshVertexBuffer()
    {
        delete m_vao;
        delete m_sphere_vao;
        delete m_cylinder_vao;
    }

    void MeshVertexBuffer::generate_buffer(Mesh& mesh)
    {
        // first update the normal face attribute for all faces
        OpenVolumeMesh::NormalAttrib normals(mesh);
        normals.update_face_normals();

        for (auto c_it : mesh.cells())
        {
            add_cell(mesh, c_it);
        }
    }

    void MeshVertexBuffer::add_cell(Mesh& mesh, Cell cell)
    {
        std::vector<FaceData> faces;
        std::vector<glm::vec3> vertices;

        // iterate over all faces of the cell
        for (auto chf_it : mesh.cell_halffaces(cell))
        {
            FaceData faceData;
            int face_id = mesh.face_handle(chf_it).idx();

            // get the normal
            auto hf_normal = mesh.normal(chf_it);

            // iterate over the halfedges of the halfface
            for (auto hfhe_it : mesh.halfface_halfedges(chf_it))
            {
                // get the corresponding edge vertex
                auto v = mesh.from_vertex_handle(hfhe_it);
                auto v_pos = mesh.vertex(v);

                VertexData v_data;
                v_data.position.x = v_pos[0];
                v_data.position.y = v_pos[1];
                v_data.position.z = v_pos[2];
                v_data.normal.x = -hf_normal[0];
                v_data.normal.y = -hf_normal[1];
                v_data.normal.z = -hf_normal[2];
                v_data.ovm_handle = v;

                vertices.push_back(v_data.position);
                faceData.vertices.push_back(v_data);

                // store ids for selection
                faceData.vertex_ids.push_back(v_data.ovm_handle.idx());
            }

            faceData.face_ids.push_back(face_id);
            int num_face_vertices = (int) faceData.vertices.size();
            if (num_face_vertices == 4)
            {
                faceData.face_ids.push_back(face_id);
            }

            add_face_indices(mesh, faceData);
            m_num_vertices += num_face_vertices;

            faces.push_back(faceData);
        }

        glm::vec3 cell_center = get_center(vertices);

        // now that we collected the data we need, we can update or buffer arrays
        for (const FaceData& face : faces)
        {
            // fill up vertex data
            for (const VertexData& vertex : face.vertices)
            {
                // position
                m_positions.push_back(vertex.position.x);
                m_positions.push_back(vertex.position.y);
                m_positions.push_back(vertex.position.z);

                // normal
                m_normals.push_back(vertex.normal.x);
                m_normals.push_back(vertex.normal.y);
                m_normals.push_back(vertex.normal.z);

                // cell center
                m_cell_centers.push_back(cell_center.x);
                m_cell_centers.push_back(cell_center.y);
                m_cell_centers.push_back(cell_center.z);
            }

            // add all indices of the face
            m_indices.insert(m_indices.end(), face.indices.begin(), face.indices.end());

            // add ids, depending on how many triangles we actually render, we need to put the id twice or more,
            // since those triangles share the same face
            m_vertex_ids.insert(m_vertex_ids.end(), face.vertex_ids.begin(), face.vertex_ids.end());
            m_edge_ids.insert(m_edge_ids.end(), face.edge_ids.begin(), face.edge_ids.end());
            m_face_ids.insert(m_face_ids.end(), face.face_ids.begin(), face.face_ids.end());
        }
    }

    void MeshVertexBuffer::add_face_indices(Mesh& mesh, FaceData& face)
    {
        switch (face.vertices.size())
        {
            case 3:
            {
                face.indices.push_back(m_num_vertices + 0);
                face.indices.push_back(m_num_vertices + 2);
                face.indices.push_back(m_num_vertices + 1);

                // for selection edge rendering, we need to store data for each edge
                add_from_to_vertex(face.vertices[0], face.vertices[2]);
                add_from_to_vertex(face.vertices[2], face.vertices[1]);
                add_from_to_vertex(face.vertices[1], face.vertices[0]);

                add_edge_id(mesh, face, 0, 2);
                add_edge_id(mesh, face, 2, 1);
                add_edge_id(mesh, face, 1, 0);
                break;
            }
            case 4:
            {
                // we have 4 vertices, so we need to create two triangles out of it
                face.indices.push_back(m_num_vertices + 0);
                face.indices.push_back(m_num_vertices + 2);
                face.indices.push_back(m_num_vertices + 1);

                face.indices.push_back(m_num_vertices + 0);
                face.indices.push_back(m_num_vertices + 3);
                face.indices.push_back(m_num_vertices + 2);

                // for selection edge rendering, we need to store data for each edge
                add_from_to_vertex(face.vertices[0], face.vertices[3]);
                add_from_to_vertex(face.vertices[3], face.vertices[2]);
                add_from_to_vertex(face.vertices[2], face.vertices[1]);
                add_from_to_vertex(face.vertices[1], face.vertices[0]);

                add_edge_id(mesh, face, 0, 3);
                add_edge_id(mesh, face, 3, 2);
                add_edge_id(mesh, face, 2, 1);
                add_edge_id(mesh, face, 1, 0);
                break;
            }
            default:
            {
                // in this case, we have undefined behavior, since we don't triangulate any further
                for (int i = 0; i < face.vertices.size(); i++)
                {
                    face.indices.push_back(m_num_vertices + i);
                    add_from_to_vertex(face.vertices[i], face.vertices[(i + 1) % face.vertices.size()]);
                }
            }
        }
    }

    void MeshVertexBuffer::add_edge_id(Mesh& mesh, FaceData& face, int idx0, int idx1)
    {
        auto vh0 = face.vertices[idx0].ovm_handle;
        auto vh1 = face.vertices[idx1].ovm_handle;

        for (auto heh: mesh.outgoing_halfedges(vh0))
        {
            auto out = mesh.to_vertex_handle(heh);
            if (out == vh1)
            {
                face.edge_ids.push_back(mesh.edge_handle(heh).idx());
                break;
            }
        }
    }

    void MeshVertexBuffer::add_from_to_vertex(const VertexData& from, const VertexData& to)
    {
        m_from_vertices.push_back(from.position.x);
        m_from_vertices.push_back(from.position.y);
        m_from_vertices.push_back(from.position.z);
        m_to_vertices.push_back(to.position.x);
        m_to_vertices.push_back(to.position.y);
        m_to_vertices.push_back(to.position.z);
    }

    glm::vec3 MeshVertexBuffer::get_center(const std::vector<glm::vec3>& vertices)
    {
        glm::vec3 min = vertices[0];
        glm::vec3 max = vertices[0];
        for (int i = 1; i < vertices.size(); i++)
        {
            const glm::vec3& vertex = vertices[i];
            if (vertex.x < min.x)
            {
                min.x = vertex.x;
            }
            else if (vertex.x > max.x)
            {
                max.x = vertex.x;
            }
            if (vertex.y < min.y)
            {
                min.y = vertex.y;
            }
            else if (vertex.y > max.y)
            {
                max.y = vertex.y;
            }
            if (vertex.z < min.z)
            {
                min.z = vertex.z;
            }
            else if (vertex.z > max.z)
            {
                max.z = vertex.z;
            }
        }
        return min + (max - min) * 0.5f;
    }

    void MeshVertexBuffer::delete_boundary_cells(Mesh& mesh)
    {
        std::vector<OpenVolumeMesh::VertexHandle> boundary_vertices;
        for (auto vh : mesh.vertices())
        {
            if (mesh.is_boundary(vh))
            {
                boundary_vertices.push_back(vh);
            }
        }
        for (auto v : boundary_vertices)
        {
            mesh.delete_vertex(v);
        }
        mesh.collect_garbage();
    }

    std::vector<float> MeshVertexBuffer::get_vertices(Mesh& mesh)
    {
        std::vector<float> vertices;
        int dim = 3;
        vertices.reserve(mesh.n_vertices() * dim);
        for (OpenVolumeMesh::VertexIter v_it = mesh.vertices_begin();
             v_it != mesh.vertices_end(); ++v_it)
        {
            auto vertex_coords = mesh.vertex(*v_it);
            for (int i = 0; i < dim; i++)
            {
                vertices.push_back(vertex_coords[i]);
            }
        }
        return vertices;
    }

    VertexArrayObject* MeshVertexBuffer::get_vao()
    {
        return m_vao;
    }

    int MeshVertexBuffer::to_vertexID(int value)
    {
        if (m_vertex_ids.size() > value)
        {
            return m_vertex_ids[value] + 1;
        }
        return 0;
    }

    int MeshVertexBuffer::to_edgeID(int value)
    {
        if (m_edge_ids.size() > value)
        {
            return m_edge_ids[value] + 1;
        }
        return 0;
    }

    int MeshVertexBuffer::to_faceID(int value)
    {
        if (m_face_ids.size() > value)
        {
            return m_face_ids[value] + 1;
        }
        return 0;
    }

    std::vector<float>& MeshVertexBuffer::get_original_vertices()
    {
        return m_original_vertices;
    }

    VertexArrayObject* MeshVertexBuffer::get_cylinder_vao()
    {
        return m_cylinder_vao;
    }

    VertexArrayObject* MeshVertexBuffer::get_sphere_vao()
    {
        return m_sphere_vao;
    }

    int MeshVertexBuffer::get_num_visible_vertices() const
    {
        return (int) m_positions.size() / 3;
    }

    int MeshVertexBuffer::get_num_visible_edges() const
    {
        return (int) m_indices.size();
    }
}
