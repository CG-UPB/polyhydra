
#include "MeshVertexBuffer.h"
#include "../rendering/meshes/CommonMeshes.h"

#include <OpenVolumeMesh/Attribs/NormalAttrib.hh>

namespace vOS
{

    MeshVertexBuffer::MeshVertexBuffer(Mesh* mesh, BufferSpecification spec): m_spec(spec)
    {
        int depth = spec.peel_depth;

        Mesh* current_mesh = mesh;
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

        m_vertices = get_vertices(*current_mesh);
        m_faces = get_indices(*current_mesh);
        auto vertex_normals = get_vertex_normals(*current_mesh);
        m_face_ids = get_face_ids(*current_mesh);
        m_edge_ids = get_edge_ids(*current_mesh, m_faces);
        auto from_to_vertices = get_from_and_to_vertices(*current_mesh, m_faces);

        m_original_vertices = m_vertices;

        m_vao = new VertexArrayObject(m_vertices, m_faces);
        m_vao->add_attribute(vertex_normals, 1, 3);

        m_sphere_vao = new VertexArrayObject(CommonMeshes::Sphere::selection_sphere().vertices(),
                                             CommonMeshes::Sphere::selection_sphere().indices());

        m_sphere_vao->add_attribute(vertex_normals, 1, 3, true);
        m_sphere_vao->add_attribute(m_vertices, 2, 3, true);

        m_cylinder_vao = new VertexArrayObject(CommonMeshes::Cylinder::edge_cylinder().vertices(),
                                               CommonMeshes::Cylinder::edge_cylinder().indices());

        m_cylinder_vao->add_attribute(from_to_vertices.first, 1, 3, true);
        m_cylinder_vao->add_attribute(from_to_vertices.second, 2, 3, true);

        if (depth > 0)
        {
            delete current_mesh;
        }

        std::cout << "num_vertices: " << m_vertices.size() << std::endl;
        std::cout << "num faces: " << m_faces.size() << std::endl;
    }

    MeshVertexBuffer::~MeshVertexBuffer()
    {
        delete m_vao;
        delete m_sphere_vao;
        delete m_cylinder_vao;
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
        std::cout << "n_vertices: " << mesh.n_vertices() << std::endl;
        std::cout << "n_cells: " << mesh.n_cells() << std::endl;
    }

    VertexArrayObject* MeshVertexBuffer::get_vao()
    {
        return m_vao;
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

    std::vector<unsigned int> MeshVertexBuffer::get_indices(Mesh& mesh)
    {
        std::vector<unsigned int> faces;

        int count;
        std::vector<int> vert_idx;

        for (OpenVolumeMesh::FaceIter f_it = mesh.faces_begin();
             f_it != mesh.faces_end(); ++f_it)
        {
            for (auto halfface: mesh.face_halffaces(*f_it))
            {
                if (!mesh.is_boundary(halfface))
                {
                    continue;
                }
                auto face_vertex_ids = mesh.halfface_vertices(halfface);

                count = 0;
                vert_idx.clear();

                //count how many vertices the face has
                for (auto fv_it = face_vertex_ids.first;
                     fv_it != face_vertex_ids.second; ++fv_it)
                {
                    count++;
                    vert_idx.push_back(fv_it->idx());
                }

                //save indices depending on count
                if (count == 3)
                {
                    // create 1 triangles out of 3 indices
                    faces.push_back(vert_idx[0]);
                    faces.push_back(vert_idx[1]);
                    faces.push_back(vert_idx[2]);
                }
                else if (count == 4)
                {
                    // create 2 triangles out of 4 indices
                    faces.push_back(vert_idx[0]);
                    faces.push_back(vert_idx[1]);
                    faces.push_back(vert_idx[2]);

                    faces.push_back(vert_idx[0]);
                    faces.push_back(vert_idx[2]);
                    faces.push_back(vert_idx[3]);
                }
                    // unpredictable behaviour
                else
                {
                    for (int i = 0; i < count; i++)
                    {
                        faces.push_back(vert_idx[i]);
                    }
                }
            }
        }
        return faces;
    }

    std::vector<unsigned int> MeshVertexBuffer::get_edge_ids(Mesh& mesh, std::vector<unsigned int>& faces)
    {
        std::vector<unsigned int> edge_ids;
        for (int i = 0; i < faces.size(); i += 3)
        {
            auto vh0 = OpenVolumeMesh::VertexHandle(faces[i + 0]);
            auto vh1 = OpenVolumeMesh::VertexHandle(faces[i + 1]);
            auto vh2 = OpenVolumeMesh::VertexHandle(faces[i + 2]);

            for (auto heh: mesh.outgoing_halfedges(vh0))
            {
                auto out = mesh.to_vertex_handle(heh);
                if (out == vh1)
                {
                    edge_ids.push_back(mesh.edge_handle(heh).idx());
                    break;
                }
            }
            for (auto heh: mesh.outgoing_halfedges(vh1))
            {
                auto out = mesh.to_vertex_handle(heh);
                if (out == vh2)
                {
                    edge_ids.push_back(mesh.edge_handle(heh).idx());
                    break;
                }
            }
            for (auto heh: mesh.outgoing_halfedges(vh2))
            {
                auto out = mesh.to_vertex_handle(heh);
                if (out == vh0)
                {
                    edge_ids.push_back(mesh.edge_handle(heh).idx());
                    break;
                }
            }
        }
        return edge_ids;
    }

    std::vector<unsigned int> MeshVertexBuffer::get_face_ids(Mesh& mesh)
    {
        std::vector<unsigned int> face_ids;
        for (OpenVolumeMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it)
        {
            for (auto halfface: mesh.face_halffaces(*f_it))
            {
                if (!mesh.is_boundary(halfface))
                {
                    continue;
                }
                face_ids.push_back(f_it->idx());
            }
        }
        return face_ids;
    }

    std::vector<float> MeshVertexBuffer::get_vertex_normals(Mesh& mesh)
    {
        std::vector<float> vertex_normals;

        OpenVolumeMesh::NormalAttrib normals(mesh);
        normals.update_face_normals();

        // normal calculation based on OpenVolumeMesh's update_vertex_normals() method
        for (const auto& _vh: mesh.vertices())
        {
            std::set<std::pair<OpenVolumeMesh::HalfFaceHandle, float>> halffaces;
            for (auto voh_it = mesh.voh_iter(_vh); voh_it.valid(); ++voh_it)
            {
                for (auto hehf_it = mesh.hehf_iter(*voh_it); hehf_it.valid(); ++hehf_it)
                {
                    // find points of this halfface to calculate the angle of the face from the vertex
                    std::vector<glm::vec3> points;
                    for (auto hfv_it : mesh.halfface_vertices(*hehf_it))
                    {
                        if (hfv_it.idx() != _vh.idx())
                        {
                            auto point = mesh.vertex(hfv_it);
                            points.emplace_back(point[0], point[1], point[2]);
                        }
                    }

                    // calculate face angle
                    float angle = M_PI * 2.0f;
                    if (points.size() == 2)
                    {
                        auto p = mesh.vertex(_vh);
                        glm::vec3 pivot = glm::vec3(p[0], p[1], p[2]);
                        glm::vec3 first = glm::normalize(points[0] - pivot);
                        glm::vec3 second = glm::normalize(points[1] - pivot);
                        angle = glm::acos(glm::dot(first, second));
                    }
                    if (mesh.is_boundary(*hehf_it))
                    {
                        halffaces.insert(std::make_pair(*hehf_it, angle));
                    }
                }
            }

            // sum up normals of adjacent faces, but assign weight based on the angle size for better results
            auto normal = glm::vec3(0.0f);
            for (auto halfface: halffaces)
            {
                auto n = mesh.normal(std::get<0>(halfface));
                float angle = std::get<1>(halfface);
                normal += glm::vec3(n[0], n[1], n[2]) * (float) (angle / M_PI * 2.0f);
            }

            auto norm = glm::normalize(normal);
            vertex_normals.push_back(norm.x);
            vertex_normals.push_back(norm.y);
            vertex_normals.push_back(norm.z);
        }
        return vertex_normals;
    }

    std::pair<std::vector<float>, std::vector<float>>
    MeshVertexBuffer::get_from_and_to_vertices(Mesh& mesh, std::vector<unsigned int>& faces)
    {
        std::vector<float> from_vertices;
        std::vector<float> to_vertices;

        for (int i = 0; i < faces.size(); i += 3)
        {
            auto vh0 = OpenVolumeMesh::VertexHandle(faces[i + 0]);
            auto vh1 = OpenVolumeMesh::VertexHandle(faces[i + 1]);
            auto vh2 = OpenVolumeMesh::VertexHandle(faces[i + 2]);

            auto v0 = mesh.vertex(vh0);
            auto v1 = mesh.vertex(vh1);
            auto v2 = mesh.vertex(vh2);

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

        return std::make_pair(from_vertices, to_vertices);
    }

    unsigned int MeshVertexBuffer::to_faceID(unsigned int value)
    {
        if (m_face_ids.size() > value)
        {
            return m_face_ids[value] + 1;
        }
        return 0;
    }

    unsigned int MeshVertexBuffer::to_edgeID(unsigned int value)
    {
        if (m_edge_ids.size() > value)
        {
            return m_edge_ids[value] + 1;
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
        return (int) m_vertices.size() / 3;
    }

    int MeshVertexBuffer::get_num_visible_edges() const
    {
        return (int) m_faces.size();
    }
}
