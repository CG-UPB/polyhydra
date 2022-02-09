
#include "MeshVertexBuffer.h"
#include "../rendering/meshes/CommonMeshes.h"

#include <OpenVolumeMesh/Attribs/NormalAttrib.hh>

namespace vOS
{

    MeshVertexBuffer::MeshVertexBuffer(Mesh* mesh, BufferSpecification spec): m_spec(spec)
    {
        int peel_depth = spec.peel_depth;
        int slice_depth = spec.slice_depth;

        Mesh* current_mesh = mesh;
        m_original_vertices = get_vertices(*current_mesh);

        generate_buffer(*current_mesh);

        m_vao = new VertexArrayObject(m_positions, m_indices);
        m_vao->add_attribute(m_normals, 1, 3);
        m_vao->add_attribute(m_cell_centers, 2, 3);
        m_vao->add_attribute(m_peel_depths, 3, 1);
        m_vao->add_attribute(m_is_face_boundary, 4, 1);

        m_sphere_vao = new VertexArrayObject(CommonMeshes::Sphere::selection_sphere().vertices(),
                                             CommonMeshes::Sphere::selection_sphere().indices());

        m_sphere_vao->add_attribute(m_normals, 1, 3, true);
        m_sphere_vao->add_attribute(m_selection_vertices, 2, 3, true);
        m_sphere_vao->add_attribute(m_sphere_cell_centers, 3, 3, true);
        m_sphere_vao->add_attribute(m_sphere_peel_depths, 4, 1, true);

        m_cylinder_vao = new VertexArrayObject(CommonMeshes::Cylinder::edge_cylinder().vertices(),
                                               CommonMeshes::Cylinder::edge_cylinder().indices());

        m_cylinder_vao->add_attribute(m_from_vertices, 1, 3, true);
        m_cylinder_vao->add_attribute(m_to_vertices, 2, 3, true);
        m_cylinder_vao->add_attribute(m_cylinder_cell_centers, 3, 3, true);
        m_cylinder_vao->add_attribute(m_cylinder_peel_depths, 4, 1, true);

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

        // add every cell to the vertex buffer
        for (auto c_it : mesh.cells())
        {
            add_cell(mesh, c_it);
        }
    }

    void MeshVertexBuffer::add_cell(Mesh& mesh, Cell cell)
    {
        OpenVolumeMesh::CellPropertyT<int> peel_property = mesh.request_cell_property<int>("PeelDepth");
        OpenVolumeMesh::VertexPropertyT<int> vertex_peel_property = mesh.request_vertex_property<int>("PeelDepth");


        std::vector<FaceData> faces;
        std::vector<glm::vec3> vertices;

        // add every vertex only once for the selection, no need to render them twice
        int num_selection_vertices = 0;
        for (auto cv_it : mesh.cell_vertices(cell))
        {
            auto v_pos = mesh.vertex(cv_it);
            vertices.emplace_back(v_pos[0], v_pos[1], v_pos[2]);
            m_selection_vertices.push_back(v_pos[0]);
            m_selection_vertices.push_back(v_pos[1]);
            m_selection_vertices.push_back(v_pos[2]);
            m_vertex_ids.push_back(cv_it.idx());
            num_selection_vertices++;
        }

        // get_rgb the center, so we can add it as a vertex attribute
        glm::vec3 cell_center = get_center(vertices);

        // get_rgb peel depth of the cell
        int peel_depth = peel_property[cell];

        for (int i = 0; i < num_selection_vertices; i++)
        {
            m_sphere_cell_centers.push_back(cell_center.x);
            m_sphere_cell_centers.push_back(cell_center.y);
            m_sphere_cell_centers.push_back(cell_center.z);

            m_sphere_peel_depths.push_back((float) peel_depth);
        }

        // same for the edges, only add them once for the selection
        for (auto ce_it : mesh.cell_edges(cell))
        {
            auto [v0, v1] = mesh.edge_vertices(ce_it);
            add_from_to_vertex(mesh, v0, v1);
            m_edge_ids.push_back(ce_it.idx());

            m_cylinder_cell_centers.push_back(cell_center.x);
            m_cylinder_cell_centers.push_back(cell_center.y);
            m_cylinder_cell_centers.push_back(cell_center.z);

            m_cylinder_peel_depths.push_back((float) peel_depth);
        }

        // now we collect the geometry data from ovm, and create data for each face of the cell individually
        for (auto chf_it : mesh.cell_halffaces(cell))
        {
            FaceData faceData;
            auto face_handle = mesh.face_handle(chf_it);
            int face_id = face_handle.idx();

            // remember if face is boundary, so that we can discard non boundary faces in the shader if needed
            if (mesh.is_boundary(face_handle))
            {
                faceData.is_boundary = true;
            }

            // get_rgb the face normal
            auto hf_normal = mesh.normal(chf_it);

            // iterate over the halfedges of the halfface
            for (auto hfhe_it : mesh.halfface_halfedges(chf_it))
            {
                // get_rgb the corresponding edge vertex
                auto v = mesh.from_vertex_handle(hfhe_it);
                auto v_pos = mesh.vertex(v);

                // get_rgb geometry data
                VertexData v_data;
                v_data.position.x = v_pos[0];
                v_data.position.y = v_pos[1];
                v_data.position.z = v_pos[2];
                v_data.normal.x = -hf_normal[0];
                v_data.normal.y = -hf_normal[1];
                v_data.normal.z = -hf_normal[2];
                v_data.ovm_handle = v;

                // if vertex lays on surface: determine normal by adjacent faces on surface
                int boundary_level = 0;
                bool phong = true;
                if (phong && peel_property[cell] == boundary_level && vertex_peel_property[v] == boundary_level)
                {
                    OpenVolumeMesh::VectorT<float, 3> normal = {0.0, 0.0, 0.0};
                    int count = 0;

                    for(auto vhf : mesh.vertex_halffaces(v))
                    {

                        auto vohf = mesh.opposite_halfface_handle(vhf);
                        auto fc = mesh.incident_cell(vhf);
                        auto ofc = mesh.incident_cell(vohf);

                        if (ofc.is_valid() && fc.is_valid() && peel_property[ofc] < peel_property[fc])
                        {
                            normal = normal + (-mesh.normal(vhf));
                            count++;
                        }
                        else if(!ofc.is_valid())
                        {
                            normal = normal + (-mesh.normal(vhf));
                            count++;
                        }
                        else if (!fc.is_valid())
                        {
                            normal = normal + (-mesh.normal(vohf));
                            count++;
                        }
                    }
                    if ( count > 0)
                    {
                        normal = normal / count;
                        v_data.normal.x = normal[0] ;
                        v_data.normal.y = normal[1];
                        v_data.normal.z = normal[2];
                    }

                }
                faceData.vertices.push_back(v_data);
            }

            // if we have a face with 4 face vertices, it gets split into 2 triangles, so we need to put the id twice
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

                m_peel_depths.push_back((float)peel_depth);
                //std::cout << peel_property[cell] <<std::endl;

                m_is_face_boundary.push_back(face.is_boundary ? 1.0f : 0.0f);
            }

            // add all indices of the face
            m_indices.insert(m_indices.end(), face.indices.begin(), face.indices.end());

            // add ids, depending on how many triangles we actually render, we need to put the id twice or more,
            // since those triangles share the same face
            m_face_ids.insert(m_face_ids.end(), face.face_ids.begin(), face.face_ids.end());
        }
    }

    void MeshVertexBuffer::add_face_indices(Mesh& mesh, FaceData& face)
    {
        switch (face.vertices.size())
        {
            case 3:
            {
                // simplest case, just connect the three vertices to a triangle
                face.indices.push_back(m_num_vertices + 0);
                face.indices.push_back(m_num_vertices + 2);
                face.indices.push_back(m_num_vertices + 1);
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
                break;
            }
            default:
            {
                // in this case, we have undefined behavior, since we don't triangulate any further
                for (int i = 0; i < face.vertices.size(); i++)
                {
                    face.indices.push_back(m_num_vertices + i);
                }
            }
        }
    }

    void MeshVertexBuffer::add_from_to_vertex(Mesh& mesh, const OpenVolumeMesh::VertexHandle& from, const OpenVolumeMesh::VertexHandle& to)
    {
        auto from_pos = mesh.vertex(from);
        auto to_pos = mesh.vertex(to);
        m_from_vertices.push_back(from_pos[0]);
        m_from_vertices.push_back(from_pos[1]);
        m_from_vertices.push_back(from_pos[2]);
        m_to_vertices.push_back(to_pos[0]);
        m_to_vertices.push_back(to_pos[1]);
        m_to_vertices.push_back(to_pos[2]);
    }

    std::pair<glm::vec3,glm::vec3> MeshVertexBuffer::get_bounding_box(const std::vector<glm::vec3>& vertices)
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
        auto bb = std::make_pair(min, max);
        return bb;
    }

    glm::vec3 MeshVertexBuffer::get_center(const std::vector<glm::vec3>& vertices)
    {
        auto bb = get_bounding_box(vertices);
        auto min = bb.first;
        auto max = bb.second;
        return min + (max - min) * 0.5f;
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

    int MeshVertexBuffer::get_num_selection_vertices() const
    {
        return (int) m_vertex_ids.size();
    }

    int MeshVertexBuffer::get_num_selection_edges() const
    {
        return (int) m_edge_ids.size();
    }
}
