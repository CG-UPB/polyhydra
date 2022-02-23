
#include "MeshVertexBuffer.h"
#include "../rendering/meshes/CommonMeshes.h"

#include <OpenVolumeMesh/Attribs/NormalAttrib.hh>

namespace vOS
{

    MeshVertexBuffer::MeshVertexBuffer(Mesh* mesh, BufferSpecification spec) : m_spec(spec)
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
        m_vao->add_attribute(m_is_digged, 5, 1);
        m_vao->add_attribute(m_colors, 6, 4);
        m_vao->add_attribute(m_is_isolated, 7, 1);
        m_vao->add_attribute(m_is_triangle, 8, 1);

        m_sphere_vao = new VertexArrayObject(CommonMeshes::Sphere::selection_sphere().vertices(),
                                             CommonMeshes::Sphere::selection_sphere().indices());

        m_sphere_vao->add_attribute(m_normals, 1, 3, true);
        m_sphere_vao->add_attribute(m_selection_vertices, 2, 3, true);
        m_sphere_vao->add_attribute(m_sphere_cell_centers, 3, 3, true);
        m_sphere_vao->add_attribute(m_sphere_peel_depths, 4, 1, true);
        m_sphere_vao->add_attribute(m_sphere_is_digged, 5, 1, true);
        m_sphere_vao->add_attribute(m_sphere_is_isolated, 6, 1, true);

        m_cylinder_vao = new VertexArrayObject(CommonMeshes::Cylinder::edge_cylinder().vertices(),
                                               CommonMeshes::Cylinder::edge_cylinder().indices());

        m_cylinder_vao->add_attribute(m_from_vertices, 1, 3, true);
        m_cylinder_vao->add_attribute(m_to_vertices, 2, 3, true);
        m_cylinder_vao->add_attribute(m_cylinder_cell_centers, 3, 3, true);
        m_cylinder_vao->add_attribute(m_cylinder_peel_depths, 4, 1, true);
        m_cylinder_vao->add_attribute(m_cylinder_is_digged, 5, 1, true);
        m_cylinder_vao->add_attribute(m_cylinder_is_isolated, 6, 1, true);

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
        for (auto c_it: mesh.cells())
        {
            add_cell(mesh, c_it);
        }
    }

    void MeshVertexBuffer::add_cell(Mesh& mesh, Cell cell)
    {
        OpenVolumeMesh::CellPropertyT<int> peel_property = mesh.request_cell_property<int>("PeelDepth");
        OpenVolumeMesh::CellPropertyT<bool> diggingProp = mesh.request_cell_property<bool>("DiggingProperty");
        OpenVolumeMesh::CellPropertyT<bool> isolateProp = mesh.request_cell_property<bool>("IsolateProperty");


        bool isDigged = diggingProp[cell];
        bool isIsolated = isolateProp[cell];


        std::vector<FaceData> faces;
        std::vector<glm::vec3> vertices;

        // add every vertex only once for the selection, no need to render them twice
        int num_selection_vertices = 0;
        for (auto cv_it: mesh.cell_vertices(cell))
        {
            auto v_pos = mesh.vertex(cv_it);
            vertices.emplace_back(v_pos[0], v_pos[1], v_pos[2]);
            m_selection_vertices.push_back(v_pos[0]);
            m_selection_vertices.push_back(v_pos[1]);
            m_selection_vertices.push_back(v_pos[2]);
            m_vertex_ids.push_back(cv_it.idx());
            num_selection_vertices++;
        }

        m_selection_sphere_digging_numbers[cell.idx()] = num_selection_vertices;

        // get the center, so we can add it as a vertex attribute
        glm::vec3 cell_center = get_center(vertices);

        // get peel depth of the cell
        int peel_depth = peel_property[cell];

        m_selection_sphere_digging_indices[cell.idx()] = (int) m_sphere_is_digged.size();
        for (int i = 0; i < num_selection_vertices; i++)
        {
            m_sphere_cell_centers.push_back(cell_center.x);
            m_sphere_cell_centers.push_back(cell_center.y);
            m_sphere_cell_centers.push_back(cell_center.z);

            m_sphere_peel_depths.push_back((float) peel_depth);

            m_sphere_is_digged.push_back(1.0f);

            m_sphere_is_isolated.push_back(1.0f);
        }

        // same for the edges, only add them once for the selection
        m_selection_cylinder_digging_indices[cell.idx()] = (int) m_cylinder_is_digged.size();
        int num_selection_edges = 0;
        for (auto ce_it: mesh.cell_edges(cell))
        {
            auto[v0, v1] = mesh.edge_vertices(ce_it);
            add_from_to_vertex(mesh, v0, v1);
            m_edge_ids.push_back(ce_it.idx());

            m_cylinder_cell_centers.push_back(cell_center.x);
            m_cylinder_cell_centers.push_back(cell_center.y);
            m_cylinder_cell_centers.push_back(cell_center.z);

            m_cylinder_peel_depths.push_back((float) peel_depth);
            m_cylinder_is_digged.push_back(1.0f);
            m_cylinder_is_isolated.push_back(1.0f);
            num_selection_edges++;
        }
        m_selection_cylinder_digging_numbers[cell.idx()] = num_selection_edges;

        // now we collect the geometry data from ovm, and create data for each face of the cell individually
        for (auto chf_it: mesh.cell_halffaces(cell))
        {
            FaceData face_data;
            auto face_handle = mesh.face_handle(chf_it);
            int face_id = face_handle.idx();

            auto hf_normal = mesh.normal(chf_it);
            auto normal = glm::vec3(hf_normal[0], hf_normal[1], hf_normal[2]);

            // remember if face is boundary, so that we can discard non boundary faces in the shader if needed
            if (mesh.is_boundary(face_handle))
            {
                face_data.is_boundary = true;
            }

            // Count the amount of Vertices this Face has
            std::vector<glm::vec3> original_face_vertices;
            for (auto hfhe_it: mesh.halfface_halfedges(chf_it))
            {
                // get the corresponding edge vertex
                auto v = mesh.from_vertex_handle(hfhe_it);
                auto v_pos = mesh.vertex(v);
                original_face_vertices.emplace_back(v_pos[0], v_pos[1], v_pos[2]);
            }

            // If it's 3 vertices, its a simple triangle, and we do not need to triangulate it further
            if (original_face_vertices.size() == 3)
            {
                // iterate over the halfedges of the halfface
                for (auto vertex_pos : original_face_vertices)
                {
                    // get geometry data
                    VertexData v_data;
                    v_data.position = vertex_pos;
                    v_data.normal = -normal;
                    face_data.vertices.push_back(v_data);
                }

                // Add face data
                face_data.face_ids.push_back(face_id);
                add_face_indices(mesh, face_data);
                m_num_vertices += (int) original_face_vertices.size();

                faces.push_back(face_data);
            }
            else if (original_face_vertices.size() > 3)
            {
                bool first_edge = true;
                glm::vec3 previous_position(0.0f);
                glm::vec3 face_normal(0.0f);

                // Triangulate Face
                // Create a new vertex that is our midpoint
                face_data.vertices.push_back(VertexData{});

                // Get Midpoint of all Vertices
                glm::vec3 midpoint(0.0f);
                for (auto vertex_pos : original_face_vertices)
                {
                    midpoint += vertex_pos;
                }
                midpoint /= original_face_vertices.size();

                // Add every other Vertex to the list, and calculate the local normals for each
                for (auto vertex_pos : original_face_vertices)
                {
                    if (!first_edge)
                    {
                        // Set three Triangle Points
                        auto pos_1 = previous_position; // Vertex Position from previous Vertex
                        auto pos_2 = vertex_pos; // This Vertice's position
                        auto pos_3 = midpoint; // Midpoint Vertex position

                        // Calculate Area of Triangle
                        auto b_a = pos_2 - pos_1;
                        auto c_a = pos_3 - pos_1;
                        auto cross = glm::cross(b_a, c_a);
                        float area = glm::length(cross) * 0.5f;

                        // Calculate Normal of Triangle
                        glm::vec3 n = glm::cross(pos_2 - pos_1, pos_3 - pos_2);

                        // Add to Face Normal and multiply by triangle area
                        face_normal += n * area;
                        previous_position = vertex_pos;
                    }
                    else
                    {
                        first_edge = false;
                        previous_position = vertex_pos;
                    }
                }
                face_normal /= -glm::length(face_normal);
                face_data.vertices[0].position = midpoint;
                face_data.vertices[0].normal = face_normal;

                // Add Vertex Data
                for (auto vertex_pos : original_face_vertices)
                {
                    VertexData v_data;
                    v_data.position = vertex_pos;
                    v_data.normal = face_normal;
                    face_data.vertices.push_back(v_data);
                    // Add face data as many times as we have vertices - 1
                    face_data.face_ids.push_back(face_id);
                }
                add_face_indices(mesh, face_data);
                m_num_vertices += (int) face_data.vertices.size();
                faces.push_back(face_data);
            }
            else
            {
                std::cout << "Face " << face_id << " has less than 3 vertices" << std::endl;
                continue;
            }
            if (m_ovm_to_gl_face_indizes.find(face_id) == m_ovm_to_gl_face_indizes.end())
                m_ovm_to_gl_face_indizes.emplace(face_id, m_face_amount++);

            if (m_start_of_cell_vertices.find(face_id) == m_start_of_cell_vertices.end())
                m_start_of_cell_vertices.emplace(face_id, m_cell_start_face_index++);

            // Add this Face to the Face Offset Array
            m_face_offset_array.push_back(m_total_vertex_count);
            m_total_vertex_count += (int) face_data.vertices.size();


            // Remember Vertex amount of face
            m_face_vertex_count.push_back((int) face_data.vertices.size());
        }

        // now that we collected the data we need, we can update or buffer arrays
        int nbr_vertices_of_cell = 0;

        m_start_of_cell_vertices[cell.idx()] = m_is_digged.size();
        for (const FaceData& face: faces)
        {
            float is_triangle = (face.vertices.size() > 3) ? 0.0f : 1.0f;

            // fill up vertex data
            for (const VertexData& vertex: face.vertices)
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

                // Color
                m_colors.push_back(1);
                m_colors.push_back(1);
                m_colors.push_back(1);
                m_colors.push_back(0);


                m_peel_depths.push_back((float) peel_depth);
                //std::cout << peel_property[cell] <<std::endl;

                m_is_digged.push_back(1.0f);
                m_is_isolated.push_back(1.0f);
                nbr_vertices_of_cell++;
                m_is_face_boundary.push_back(face.is_boundary ? 1.0f : 0.0f);
                m_is_triangle.push_back(is_triangle);
            }

            // add all indices of the face
            m_indices.insert(m_indices.end(), face.indices.begin(), face.indices.end());

            // add ids, depending on how many triangles we actually render, we need to put the id twice or more,
            // since those triangles share the same face
            m_face_ids.insert(m_face_ids.end(), face.face_ids.begin(), face.face_ids.end());
        }
        m_size_of_cell_vertices[cell.idx()] = nbr_vertices_of_cell;

    }

    void MeshVertexBuffer::add_face_indices(Mesh& mesh, FaceData& face) const
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
            default:
            {

                // Triangulate in such a way, that every triangle uses the midpoint (with id 0) is part of the triangle
                for (int i = 0; i < face.vertices.size() - 2; i++)
                {
                    face.indices.push_back(m_num_vertices);
                    face.indices.push_back(m_num_vertices + i + 2);
                    face.indices.push_back(m_num_vertices + i + 1);
                }

                // The Last Triangle Vertex IDs loop back around
                face.indices.push_back(m_num_vertices);
                face.indices.push_back(m_num_vertices + 1);
                face.indices.push_back(m_num_vertices + face.vertices.size() - 1);
                break;
            }
        }
    }

    void MeshVertexBuffer::set_face_color(int ovm_id, float r, float g, float b, float a)
    {
        // Out of Bounce Check
        if (ovm_id < 0 || ovm_id > m_ovm_to_gl_face_indizes.size())
            return;

        int buffer_index = m_ovm_to_gl_face_indizes[ovm_id];
        int vertex_count = m_face_vertex_count[buffer_index];
        int offset_index = m_face_offset_array[m_ovm_to_gl_face_indizes[ovm_id]];
        int color_array_index = offset_index * 4;

        for (int i = 0; i < vertex_count; i++)
        {
            m_colors[color_array_index + (i * 4)] = r;
            m_colors[color_array_index + (i * 4) + 1] = g;
            m_colors[color_array_index + (i * 4) + 2] = b;
            m_colors[color_array_index + (i * 4) + 3] = a;
        }
        m_update_vao = true;
    }

    void MeshVertexBuffer::add_from_to_vertex(Mesh& mesh, const OpenVolumeMesh::VertexHandle& from,
                                              const OpenVolumeMesh::VertexHandle& to)
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

    std::pair<glm::vec3, glm::vec3> MeshVertexBuffer::get_bounding_box(const std::vector<glm::vec3>& vertices)
    {
        glm::vec3 min = vertices[0];
        glm::vec3 max = vertices[0];
        for (int i = 1; i < vertices.size(); i++)
        {
            const glm::vec3& vertex = vertices[i];
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
        if (m_update_vao)
        {
            m_vao->update_attribute(m_colors, 6);
            m_update_vao = false;
        }
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

    void MeshVertexBuffer::update_digging_buffer(int id, float newValue)
    {
        int nbr_vertices = m_size_of_cell_vertices[id];
        int start = m_start_of_cell_vertices[id];

        for (size_t i = 0; i < nbr_vertices; i++)
        {
            m_is_digged[start + i] = newValue;
        }

        int sphere_index = m_selection_sphere_digging_indices[id];
        int nbr_spheres = m_selection_sphere_digging_numbers[id];
        for (size_t i = 0; i < nbr_spheres; i++)
        {
            m_sphere_is_digged[sphere_index + i] = newValue;
        }

        int cylinder_index = m_selection_cylinder_digging_indices[id];
        int nbr_cylinders = m_selection_cylinder_digging_numbers[id];
        for (size_t i = 0; i < nbr_cylinders; i++)
        {
            m_cylinder_is_digged[cylinder_index + i] = newValue;
        }

        m_vao->update_attribute(m_is_digged, 5);
        m_sphere_vao->update_attribute(m_sphere_is_digged, 5);
        m_cylinder_vao->update_attribute(m_cylinder_is_digged, 5);
    }

    void MeshVertexBuffer::reset_digging()
    {
        for (size_t i = 0; i < m_is_digged.size(); i++)
        {
            m_is_digged[i] = 1.0;
        }
        for (size_t i = 0; i < m_sphere_is_digged.size(); i++)
        {
            m_sphere_is_digged[i] = 1.0;
        }
        for (size_t i = 0; i < m_cylinder_is_digged.size(); i++)
        {
            m_cylinder_is_digged[i] = 1.0;
        }

        m_vao->update_attribute(m_is_digged, 5);
        m_sphere_vao->update_attribute(m_sphere_is_digged, 5);
        m_cylinder_vao->update_attribute(m_cylinder_is_digged, 5);
    }


    void MeshVertexBuffer::update_isolate_buffer(int id, float newValue)
    {
        int nbr_vertices = m_size_of_cell_vertices[id];
        int start = m_start_of_cell_vertices[id];

        for (size_t i = 0; i < nbr_vertices; i++)
        {
            m_is_isolated[start + i] = newValue;
        }

        int sphere_index = m_selection_sphere_digging_indices[id];
        int nbr_spheres = m_selection_sphere_digging_numbers[id];
        for (size_t i = 0; i < nbr_spheres; i++)
        {
            m_sphere_is_isolated[sphere_index + i] = newValue;
        }

        int cylinder_index = m_selection_cylinder_digging_indices[id];
        int nbr_cylinders = m_selection_cylinder_digging_numbers[id];
        for (size_t i = 0; i < nbr_cylinders; i++)
        {
            m_cylinder_is_isolated[cylinder_index + i] = newValue;
        }

        activate_isolation();
    }

    void MeshVertexBuffer::start_isolation()
    {
        for (size_t i = 0; i < m_is_isolated.size(); i++)
        {
            m_is_isolated[i] = 0.0;
        }
        for (size_t i = 0; i < m_sphere_is_isolated.size(); i++)
        {
            m_sphere_is_isolated[i] = 0.0;
        }
        for (size_t i = 0; i < m_cylinder_is_isolated.size(); i++)
        {
            m_cylinder_is_isolated[i] = 0.0;
        }
        //activate_isolation();
    }

    void MeshVertexBuffer::activate_isolation()
    {
        m_vao->update_attribute(m_is_isolated, 7);
        m_sphere_vao->update_attribute(m_sphere_is_isolated, 6);
        m_cylinder_vao->update_attribute(m_cylinder_is_isolated, 6);
    }

    void MeshVertexBuffer::reset_isolation()
    {
        for (size_t i = 0; i < m_is_isolated.size(); i++)
        {
            m_is_isolated[i] = 1.0;
        }
        for (size_t i = 0; i < m_sphere_is_isolated.size(); i++)
        {
            m_sphere_is_isolated[i] = 1.0;
        }
        for (size_t i = 0; i < m_cylinder_is_isolated.size(); i++)
        {
            m_cylinder_is_isolated[i] = 1.0;
        }

        m_vao->update_attribute(m_is_isolated, 7);
        m_sphere_vao->update_attribute(m_sphere_is_isolated, 6);
        m_cylinder_vao->update_attribute(m_cylinder_is_isolated, 6);
    }

}
