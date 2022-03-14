
#include "MeshVertexBuffer.h"
#include "../rendering/meshes/CommonMeshes.h"
#include "../util/VecUtil.h"
#include "MeshProperties.h"
#include <unordered_set>
#include <type_traits>

namespace vOS
{
    AttributeDefinitions MeshVertexBuffer::s_attribute_definitions{};

    AttributeDefinitions::AttributeDefinitions()
    {
        auto mesh_vaos = {VAO::MESH_FACE, VAO::MESH_ROUNDED};
        define_attribute(Attribute::POSITION, {0, 3, false}, mesh_vaos);
        define_attribute(Attribute::NORMAL, {1, 3, false}, mesh_vaos);
        define_attribute(Attribute::CELL_CENTER, {2, 3, false}, mesh_vaos);
        define_attribute(Attribute::PEEL_DEPTH, {3, 1, false}, mesh_vaos);
        define_attribute(Attribute::IS_DIGGED, {4, 1, false}, mesh_vaos);
        define_attribute(Attribute::COLOR, {5, 4, false}, mesh_vaos);
        define_attribute(Attribute::IS_ISOLATED, {6, 1, false}, mesh_vaos);
        define_attribute(Attribute::IS_TRIANGLE, {7, 1, false}, mesh_vaos);
        define_attribute(Attribute::VERTEX_TYPE, {8, 1, false}, mesh_vaos);
        define_attribute(Attribute::FACE_CENTER, {9, 3, false}, mesh_vaos);
        define_attribute(Attribute::TO_VERTEX, {10, 3, false}, mesh_vaos);
        define_attribute(Attribute::DIHEDRAL_ANGLE, {11, 1, false}, mesh_vaos);
        define_attribute(Attribute::SELECTION, {12, 1, false}, mesh_vaos);
        define_attribute(Attribute::HOVERED, {13, 1, false}, mesh_vaos);

        auto sphere_vaos = {VAO::SPHERE};
        define_attribute(Attribute::POSITION, {0, 3, false}, sphere_vaos);
        define_attribute(Attribute::NORMAL, {1, 3, true}, sphere_vaos);
        define_attribute(Attribute::SELECTION_VERTEX_POSITION, {2, 3, true}, sphere_vaos);
        define_attribute(Attribute::CELL_CENTER, {3, 3, true}, sphere_vaos);
        define_attribute(Attribute::PEEL_DEPTH, {4, 1, true}, sphere_vaos);
        define_attribute(Attribute::IS_DIGGED, {5, 1, true}, sphere_vaos);
        define_attribute(Attribute::IS_ISOLATED, {6, 1, true}, sphere_vaos);

        auto cylinder_vaos = {VAO::CYLINDER};
        define_attribute(Attribute::POSITION, {0, 3, false}, cylinder_vaos);
        define_attribute(Attribute::SELECTION_FROM_VERTEX, {1, 3, true}, cylinder_vaos);
        define_attribute(Attribute::SELECTION_TO_VERTEX, {2, 3, true}, cylinder_vaos);
        define_attribute(Attribute::CELL_CENTER, {3, 3, true}, cylinder_vaos);
        define_attribute(Attribute::PEEL_DEPTH, {4, 1, true}, cylinder_vaos);
        define_attribute(Attribute::IS_DIGGED, {5, 1, true}, cylinder_vaos);
        define_attribute(Attribute::IS_ISOLATED, {6, 1, true}, cylinder_vaos);
    }

    MeshVertexBuffer::MeshVertexBuffer(Mesh* mesh):
        m_mesh(*mesh), m_current_loading_cell_it(mesh->cells_begin()), m_normals(*mesh)
    {
        // first update the normal face attribute for all faces
        m_normals.update_vertex_normals();
        m_original_vertices = get_vertices(m_mesh);
        m_loading_start = std::chrono::steady_clock::now();
    }

    MeshVertexBuffer::~MeshVertexBuffer()
    {
        delete m_vao.face;
        delete m_vao.rounded;
        delete m_vao.selection_sphere;
        delete m_vao.selection_cylinder;
        delete m_vao.vertex_only;
    }

    void MeshVertexBuffer::build_vertex_arrays()
    {
        auto& positions_by_face = get_attrib_array(VAO::MESH_FACE, Attribute::POSITION);
        m_vao.face = new VertexArrayObject(positions_by_face, m_indices_face);
        add_vao_attributes(m_vao.face, VAO::MESH_FACE);

        auto& positions_rounded = get_attrib_array(VAO::MESH_ROUNDED, Attribute::POSITION);
        m_vao.rounded = new VertexArrayObject(positions_rounded, m_indices_rounded);
        add_vao_attributes(m_vao.rounded, VAO::MESH_ROUNDED);

        m_vao.selection_sphere = new VertexArrayObject(CommonMeshes::Sphere::selection_sphere().vertices(),
                                                       CommonMeshes::Sphere::selection_sphere().indices());
        add_vao_attributes(m_vao.selection_sphere, VAO::SPHERE);

        m_vao.selection_cylinder = new VertexArrayObject(CommonMeshes::Cylinder::edge_cylinder().vertices(),
                                                         CommonMeshes::Cylinder::edge_cylinder().indices());
        add_vao_attributes(m_vao.selection_cylinder, VAO::CYLINDER);

        m_vao.vertex_only = new VertexArrayObject(CommonMeshes::Sphere::vertices(),CommonMeshes::Sphere::indices());
        add_vao_attributes(m_vao.vertex_only, VAO::SPHERE);
    }

    std::vector<float>& MeshVertexBuffer::get_attrib_array(VAO vao, Attribute attribute)
    {
        auto& vao_locations = s_attribute_definitions.of(vao);
        auto& location = vao_locations[static_cast<int>(attribute)];
        auto& vao_data = m_attributes[static_cast<int>(vao)];
        return vao_data.data[location.location];
    }

    void MeshVertexBuffer::add_attribute_data(VAO vao, Attribute attribute, const std::vector<float>& data)
    {
        VecUtil::push_buffer(data, get_attrib_array(vao, attribute));
    }

    void MeshVertexBuffer::add_vao_attributes(VertexArrayObject* vao, VAO vao_id)
    {
        auto& vao_locations = s_attribute_definitions.of(vao_id);
        for (auto& data : vao_locations)
        {
            auto& attribute = data.second;
            // skip since we already added the position attribute in the vertex array constructor
            if (attribute.location == 0)
            {
                continue;
            }
            auto& vao_data = m_attributes[static_cast<int>(vao_id)];
            auto& attribute_data = vao_data.data[attribute.location];
            if (!attribute_data.empty())
            {
                vao->add_attribute(
                        attribute_data,
                        attribute.location,
                        attribute.element_count,
                        attribute.per_instance
                );
            }
        }
        m_vertex_arrays[static_cast<int>(vao_id)].push_back(vao);
    }

    std::pair<int, int>& MeshVertexBuffer::get_halfface_index_and_count(VAO vao, int halfface_id)
    {
        return m_attributes[static_cast<int>(vao)].halfface_index_and_count[halfface_id];
    }

    void MeshVertexBuffer::add_halfface_index_and_count(VAO vao, int halfface_id, int index, int count)
    {
        m_attributes[static_cast<int>(vao)].halfface_index_and_count[halfface_id] = std::make_pair(index, count);
    }

    std::pair<int, int>& MeshVertexBuffer::get_cell_index_and_count(VAO vao, int cell_id)
    {
        return m_attributes[static_cast<int>(vao)].cell_index_and_count[cell_id];
    }

    void MeshVertexBuffer::add_cell_index_and_count(VAO vao, int cell_id, int index, int count)
    {
        m_attributes[static_cast<int>(vao)].cell_index_and_count[cell_id] = std::make_pair(index, count);
    }

    void MeshVertexBuffer::load_next_cell()
    {
        if (m_is_loading_finished)
        {
            return;
        }
        if (m_current_loading_cell_it->is_valid() && m_current_loading_cell_it != m_mesh.cells_end())
        {
            add_cell_by_faces(m_mesh, *m_current_loading_cell_it);
            add_cell_rounded(m_mesh, *m_current_loading_cell_it);
            m_current_loading_cell_it++;
            m_num_loaded_cells++;
        }
        else
        {
            m_is_loading_finished = true;
            m_average_cell_size /= (float) m_mesh.n_cells();
            build_vertex_arrays();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - m_loading_start
            ).count();
            std::cout << "Loading mesh took " << (float) duration / 1000.0f << " seconds" << std::endl;
        }
    }

    bool MeshVertexBuffer::is_loading_finished() const
    {
        return m_is_loading_finished;
    }

    float MeshVertexBuffer::get_loading_percentage()
    {
        return ((float) m_num_loaded_cells / (float) m_mesh.n_cells()) * 100.0f;
    }

    void MeshVertexBuffer::add_cell_by_faces(Mesh& mesh, Cell cell)
    {
        auto peel_property = mesh.request_cell_property<int>(MeshProperties::PROP_PEEL_DEPTH);

        std::vector<HalffaceData> halffaces;
        std::vector<glm::vec3> vertices;

        // add every vertex only once for the selection, no need to render them twice
        int num_selection_vertices = 0;
        for (auto cv_it: mesh.cell_vertices(cell))
        {
            auto v_pos = VecUtil::pos_to_vec3(mesh, cv_it);
            vertices.push_back(v_pos);
            VecUtil::push_vec3(get_attrib_array(VAO::SPHERE, Attribute::SELECTION_VERTEX_POSITION), v_pos);
            m_selection_map.vertex_ids.push_back(cv_it.idx());
            num_selection_vertices++;
        }

        // get the center, so we can add it as a vertex attribute
        auto [min, max] = VecUtil::get_bounding_box(vertices);
        glm::vec3 diameter = max - min;
        float size = std::max(std::max(diameter.x, diameter.y), diameter.z);
        m_average_cell_size += size;
        glm::vec3 cell_center = VecUtil::get_center(vertices);
        m_cell_centers[cell.idx()] = cell_center;

        max_bounding_box = max;
        min_bounding_box = min;

        // get peel depth of the cell
        int peel_depth = peel_property[cell];
        m_peel_depths[cell.idx()] = (float) peel_depth;
        for (int i = 0; i < num_selection_vertices; i++)
        {
            VecUtil::push_vec3(get_attrib_array(VAO::SPHERE, Attribute::CELL_CENTER), cell_center);
            get_attrib_array(VAO::SPHERE, Attribute::PEEL_DEPTH).push_back((float) peel_depth);
            get_attrib_array(VAO::SPHERE, Attribute::IS_DIGGED).push_back(0.0f);
            get_attrib_array(VAO::SPHERE, Attribute::IS_ISOLATED).push_back(0.0f);
        }
        add_cell_index_and_count(VAO::SPHERE, cell.idx(), m_vertex_offset_sphere, num_selection_vertices);
        m_vertex_offset_sphere += num_selection_vertices;

        // same for the edges, only add them once for the selection
        int num_selection_edges = 0;
        for (auto ce_it: mesh.cell_edges(cell))
        {
            auto[v0, v1] = mesh.edge_vertices(ce_it);
            add_from_to_vertex(mesh, v0, v1);
            m_selection_map.edge_ids.push_back(ce_it.idx());
            VecUtil::push_vec3(get_attrib_array(VAO::CYLINDER, Attribute::CELL_CENTER), cell_center);
            get_attrib_array(VAO::CYLINDER, Attribute::PEEL_DEPTH).push_back((float) peel_depth);
            get_attrib_array(VAO::CYLINDER, Attribute::IS_DIGGED).push_back(0.0f);
            get_attrib_array(VAO::CYLINDER, Attribute::IS_ISOLATED).push_back(0.0f);
            num_selection_edges++;
        }
        add_cell_index_and_count(VAO::CYLINDER, cell.idx(), m_vertex_offset_cylinder, num_selection_edges);
        m_vertex_offset_cylinder += num_selection_edges;

        int cell_vertex_offset = m_vertex_offset_face;

        // now we collect the geometry data from ovm, and create data for each face of the cell individually
        for (auto chf_it: mesh.cell_halffaces(cell))
        {
            HalffaceData halfface_data;
            int halfface_id = chf_it.idx();
            auto normal = normal_to_vec3(chf_it.idx());

            // Count the amount of Vertices this Face has
            std::vector<glm::vec3> halfface_vertices;
            for (auto hfhe_it: mesh.halfface_halfedges(chf_it))
            {
                // get the corresponding edge vertex
                halfface_vertices.push_back(VecUtil::pos_to_vec3(mesh, mesh.from_vertex_handle(hfhe_it)));
            }

            // If it's 3 vertices, its a simple triangle, and we do not need to triangulate it further
            if (halfface_vertices.size() == 3)
            {
                glm::vec3 barycenter = {0,0,0};
                // iterate over the halfedges of the halfface
                for (auto vertex_pos: halfface_vertices)
                {
                    // get geometry data
                    VertexData v_data;
                    v_data.position = vertex_pos;
                    barycenter += vertex_pos;
                    v_data.normal = -normal;
                    halfface_data.vertices.push_back(v_data);
                }
                halfface_data.halfface_ids.push_back(halfface_id);

                // Add face data
                add_face_indices(mesh, halfface_data);
                m_num_vertices_face += (int) halfface_vertices.size();
                m_face_centers.emplace(halfface_id, barycenter / 3.0f);
                m_face_normals.emplace(halfface_id, normal);
                halffaces.push_back(halfface_data);
            }
            else if (halfface_vertices.size() > 3)
            {
                // Triangulate Face
                // Create a new vertex that is our midpoint
                halfface_data.vertices.push_back(VertexData{});

                // Get Midpoint of all Vertices
                glm::vec3 midpoint(0.0f);
                for (auto vertex_pos: halfface_vertices)
                {
                    midpoint += vertex_pos;
                }
                midpoint /= halfface_vertices.size();

                // Add every other Vertex to the list, and calculate the local normals for each
                bool first_edge = true;
                glm::vec3 prev_pos(0.0f);
                glm::vec3 hf_normal(0.0f);
                for (auto vertex_pos: halfface_vertices)
                {
                    if (!first_edge)
                    {
                        // add cross product without normalization. since the length of the cross product is
                        // proportional to the area of the triangle, we can use that as weight
                        hf_normal += glm::cross(vertex_pos - prev_pos, midpoint - vertex_pos);
                    } else
                    {
                        first_edge = false;
                    }
                    prev_pos = vertex_pos;
                }
                hf_normal = -glm::normalize(hf_normal);
                halfface_data.vertices[0].position = midpoint;
                halfface_data.vertices[0].normal = hf_normal;

                m_face_centers.emplace(halfface_id, midpoint);
                m_face_normals.emplace(halfface_id, hf_normal);

                // Add Vertex Data
                for (auto vertex_pos: halfface_vertices)
                {
                    VertexData v_data;
                    v_data.position = vertex_pos;
                    v_data.normal = hf_normal;
                    halfface_data.vertices.push_back(v_data);
                    halfface_data.halfface_ids.push_back(halfface_id);
                }
                add_face_indices(mesh, halfface_data);
                m_num_vertices_face += (int) halfface_data.vertices.size();
                halffaces.push_back(halfface_data);
            }
            add_halfface_index_and_count(VAO::MESH_FACE, halfface_id, m_vertex_offset_face, (int) halfface_data.vertices.size());
            m_vertex_offset_face += (int) halfface_data.vertices.size();
        }

        // now that we collected the data we need, we can update or buffer arrays
        int cell_vertex_count = 0;
        for (const HalffaceData& halfface: halffaces)
        {
            float is_triangle = (halfface.vertices.size() > 3) ? 0.0f : 1.0f;

            // fill up vertex data
            for (const VertexData& vertex: halfface.vertices)
            {
                VecUtil::push_vec3(get_attrib_array(VAO::MESH_FACE, Attribute::POSITION), vertex.position);
                VecUtil::push_vec3(get_attrib_array(VAO::MESH_FACE, Attribute::NORMAL), vertex.normal);
                VecUtil::push_vec3(get_attrib_array(VAO::MESH_FACE, Attribute::CELL_CENTER), cell_center);
                VecUtil::push_vec4(get_attrib_array(VAO::MESH_FACE, Attribute::COLOR), glm::vec4{1.0f, 1.0f, 1.0f, 0.0f});
                get_attrib_array(VAO::MESH_FACE, Attribute::PEEL_DEPTH).push_back((float) peel_depth);
                get_attrib_array(VAO::MESH_FACE, Attribute::IS_DIGGED).push_back(0.0f);
                get_attrib_array(VAO::MESH_FACE, Attribute::IS_ISOLATED).push_back(0.0f);
                get_attrib_array(VAO::MESH_FACE, Attribute::IS_TRIANGLE).push_back(is_triangle);
                get_attrib_array(VAO::MESH_FACE, Attribute::SELECTION).push_back(0.0f);
                get_attrib_array(VAO::MESH_FACE, Attribute::HOVERED).push_back(0.0f);
                cell_vertex_count++;
            }

            // add all indices of the halfface
            VecUtil::push_buffer(halfface.indices, m_indices_face);

            // add ids, depending on how many triangles we actually render, we need to put the id twice or more,
            // since those triangles share the same halfface
            VecUtil::push_buffer(halfface.halfface_ids, m_selection_map.halfface_ids);
        }
        add_cell_index_and_count(VAO::MESH_FACE, cell.idx(), cell_vertex_offset, cell_vertex_count);
    }

    void MeshVertexBuffer::add_face_indices(Mesh& mesh, HalffaceData& face) const
    {
        switch (face.vertices.size())
        {
            case 3:
            {
                // simplest case, just connect the three vertices to a triangle
                face.indices.push_back(m_num_vertices_face + 0);
                face.indices.push_back(m_num_vertices_face + 2);
                face.indices.push_back(m_num_vertices_face + 1);
                break;
            }
            default:
            {

                // Triangulate in such a way, that every triangle uses the midpoint (with id 0) is part of the triangle
                for (int i = 0; i < face.vertices.size() - 2; i++)
                {
                    face.indices.push_back(m_num_vertices_face);
                    face.indices.push_back(m_num_vertices_face + i + 2);
                    face.indices.push_back(m_num_vertices_face + i + 1);
                }

                // The Last Triangle Vertex IDs loop back around
                face.indices.push_back(m_num_vertices_face);
                face.indices.push_back(m_num_vertices_face + 1);
                face.indices.push_back(m_num_vertices_face + face.vertices.size() - 1);
                break;
            }
        }
    }

    unsigned int MeshVertexBuffer::add_vertex_data_to_cell_data(
            RoundedCellData& data,
            const float type,
            const glm::vec3& pos,
            const glm::vec3& norm,
            const glm::vec4& col,
            const glm::vec3& face_center,
            const glm::vec3& to_vertex,
            float dihedral_angle)
    {
        auto& cell_center = m_cell_centers[data.cell_id];
        auto peel_depth = m_peel_depths[data.cell_id];
        unsigned int index = data.vertex_types.size();
        VecUtil::push_vec3(data.vertex_positions, pos);
        VecUtil::push_vec3(data.vertex_normals, norm);
        VecUtil::push_vec3(data.vertex_cell_centers, cell_center);
        VecUtil::push_vec4(data.vertex_colors, col);
        data.vertex_peel_depths.push_back(peel_depth);
        data.vertex_is_triangle.push_back(1.0f);
        data.vertex_is_digged.push_back(0.0f);
        data.vertex_is_isolated.push_back(0.0f);
        data.vertex_types.push_back(type);
        VecUtil::push_vec3(data.face_center, face_center);
        VecUtil::push_vec3(data.to_vertex, to_vertex);
        data.dihedral_angle.push_back(dihedral_angle);
        data.selection.push_back(0.0f);
        data.hovered.push_back(0.0f);
        return index;
    }

    void MeshVertexBuffer::add_cell_triangle_indices(RoundedCellData& data, unsigned int i0, unsigned int i1, unsigned int i2) const
    {
        data.indices.push_back(m_current_rounded_index + i0);
        data.indices.push_back(m_current_rounded_index + i2);
        data.indices.push_back(m_current_rounded_index + i1);
    }

    void MeshVertexBuffer::add_cell_rounded(Mesh& mesh, Cell cell)
    {
        int cell_vertex_offset = m_vertex_offset_rounded;

        std::unordered_map<int, glm::vec3> face_centers;
        std::unordered_map<int, glm::vec3> face_normals;
        std::unordered_map<int, std::vector<RoundedVertexData>> cell_vertices;
        // start with the halffaces, because with them, we can navigate inside the current cell without other cells
        // if we would instead take the halfedges of a vertex for example, they would include other cells,
        // which we don't want
        for (auto chf_it: mesh.cell_halffaces(cell))
        {
            face_normals[chf_it.idx()] = VecUtil::normal_to_vec3(mesh, chf_it);
            glm::vec3 center(0.0f);
            int num_vertices = 0;
            // first, get the center of the halfface
            for (auto hfv_it: mesh.halfface_vertices(chf_it))
            {
                center += VecUtil::pos_to_vec3(mesh, hfv_it);
                num_vertices++;
            }
            face_centers[chf_it.idx()] = center / (float) num_vertices;
            // This is a bit tricky, for each vertex we want a way to iterate over all adjacent edges, but ONLY within
            // the current cell. And additionally, they need to be in order, meaning all edges that lie next to each other
            // in our vector must share the same halfface. Basically, imagine a halfface circle around each vertex.
            // This is important when we want to calculate the edge normals, for example.
            for (auto hfhe_it: mesh.halfface_halfedges(chf_it))
            {
                // this is the corner vertex we are currently looking at
                auto from_vertex = mesh.from_vertex_handle(hfhe_it);
                // since we are iterating over the halfedges, we can get vertices twice, so just check the next one
                if (cell_vertices.find(from_vertex.idx()) != cell_vertices.end())
                {
                    continue;
                }
                // this is the data we need to store so that we can iterate over the vertices with more information later
                // basically, store the outgoing halfedge, and one adjacent halfface of the edge
                // from
                // |    \
                // he    \
                // |   hf \
                // to -----*
                cell_vertices[from_vertex.idx()].push_back({
                       from_vertex.idx(),
                       mesh.to_vertex_handle(hfhe_it).idx(),
                       hfhe_it.idx(),
                       chf_it.idx()
                });
                auto current_halfedge = hfhe_it;
                auto current_halfface = chf_it;
                do
                {
                    // with this, we can get the next halfface within the cell, like this
                    //              *
                    //           /  |  \
                    //        /     he    \
                    //     /        |        \
                    //  /       *------->      \
                    // *------------*------------*
                    current_halfface = mesh.adjacent_halfface_in_cell(current_halfface, current_halfedge);
                    // exit condition, we have closed the loop
                    if (current_halfface == chf_it)
                    {
                        break;
                    }
                    // now that we have our next halfface, we need to find the associated halfedge that starts at
                    // the corner vertex within the halfface
                    for (auto chfe_it: mesh.halfface_edges(current_halfface))
                    {
                        auto [he0, he1] = mesh.edge_halfedges(chfe_it);
                        auto from0 = mesh.from_vertex_handle(he0);
                        auto from1 = mesh.from_vertex_handle(he1);
                        if (from0 == from_vertex && current_halfedge != he0)
                        {
                            // we have found our halfedge
                            current_halfedge = he0;
                            cell_vertices[from_vertex.idx()].push_back({
                                   from_vertex.idx(),
                                   mesh.to_vertex_handle(current_halfedge).idx(),
                                   current_halfedge.idx(),
                                   current_halfface.idx()
                            });
                            break;
                        }
                        if (from1 == from_vertex && current_halfedge != he1)
                        {
                            // we have found our halfedge
                            current_halfedge = he1;
                            cell_vertices[from_vertex.idx()].push_back({
                                   from_vertex.idx(),
                                   mesh.to_vertex_handle(current_halfedge).idx(),
                                   current_halfedge.idx(),
                                   current_halfface.idx()
                            });
                            break;
                        }
                    }
                } while (current_halfface != chf_it);
            }
        }

        // now we need some maps to store the indices of the vertices we are going to create. We need to iterate
        // over them later and use these maps to tell opengl which ones to connect as triangles.
        // basically, almost all of them map from ovm id to opengl index

        // for each halfface the vertex that lies in the center of the halfface
        std::unordered_map<int, unsigned int> face_center_indices;
        // the index for each corner vertex of the cell
        std::unordered_map<int, unsigned int> corner_vertex_indices;
        // for each halfface those vertices that are close to the edge, basically the smaller triangle in the middle
        std::unordered_map<int, std::vector<RoundedFaceVertexData>> halfface_vertices_indices;
        // for each corner vertex of the halfface, the vertex on the inner triangle that is closest to the corner
        std::unordered_map<int, std::unordered_map<int, unsigned int>> corner_vertex_face_vertex_index;
        // for each halfedge, and for a given corner vertex, the new vertex on the halfedge that is closest to the corner
        // *-----*---------he----------*-----*
        //       ^                     ^
        //       |                     |
        // this one for           this one for
        // the left corner        the right corner
        std::unordered_map<int, std::unordered_map<int, unsigned int>> halfedge_vertex_indices;


        struct VertexAttribData
        {
            float type;
            glm::vec3 pos;
            glm::vec3 norm;
            glm::vec4 col;
            glm::vec3 face_center;
            glm::vec3 to_vertex;
            float dihedral_angle;
            RoundedFaceVertexData data;

        };
        std::unordered_map<int, std::vector<VertexAttribData>> halfface_vertices;

        RoundedCellData cell_data;
        cell_data.cell_id = cell.idx();
        glm::vec4 color(1.0f, 1.0f, 1.0f, 0.0f);
        glm::vec3 zero(0.0f);
        int total_cell_vertex_count = 0;
        // iterate over the vertices that we have collected earlier
        for (auto& it: cell_vertices)
        {
            // ovm id of the corner vertex
            const int vertex_id = it.first;
            // the outgoing halfedges and halffaces in a circle around the corner
            const auto& vertex_data = it.second;
            glm::vec3 corner_normal(0.0f);
            glm::vec3 corner_pos = VecUtil::pos_to_vec3(mesh, vertex_id);
            glm::vec3 corner_face_center_average(0.0f);
            float corner_dihedral_angle_average = 0.0f;
            for (size_t i = 0; i < vertex_data.size(); i++)
            {
                auto& prev_data = vertex_data[(i + 2) % vertex_data.size()];
                auto& data = vertex_data[(i + 1) % vertex_data.size()];
                auto& next_data = vertex_data[i];

                glm::vec3 to_vertex_pos = VecUtil::pos_to_vec3(mesh, data.to_vertex_id);
                glm::vec3 next_to_vertex_pos = VecUtil::pos_to_vec3(mesh, next_data.to_vertex_id);

                corner_normal += glm::cross(to_vertex_pos - corner_pos, next_to_vertex_pos - corner_pos);
                corner_face_center_average += face_centers[data.halfface_id];

                glm::vec3 face_normal = normal_to_vec3(data.halfface_id);
                glm::vec3 prev_face_normal = normal_to_vec3(prev_data.halfface_id);

                float dihedral_angle = M_PI - VecUtil::get_angle(face_normal, prev_face_normal);
                corner_dihedral_angle_average += dihedral_angle;

                glm::vec3 edge_face_center_average = (face_centers[data.halfface_id] + face_centers[prev_data.halfface_id]) * 0.5f;
                glm::vec3 edge_normal = glm::normalize(face_normal + prev_face_normal);

                auto edge = vOS::Mesh::edge_handle(OpenVolumeMesh::HalfEdgeHandle{data.halfedge_id});
                // edge vertex
                halfedge_vertex_indices[edge.idx()][data.from_vertex_id] = add_vertex_data_to_cell_data(
                        cell_data,
                        VertexType::EDGE,
                        corner_pos,
                        -edge_normal,
                        color,
                        edge_face_center_average,
                        to_vertex_pos,
                        dihedral_angle
                );
                total_cell_vertex_count++;
                // face vertex
                glm::vec3 face_center = face_centers[data.halfface_id];
                halfface_vertices[data.halfface_id].push_back({
                  VertexType::FACE,
                  corner_pos,
                  -face_normal,
                  color,
                  face_center,
                  zero,
                  0.0f,
                  {
                          0,
                          vertex_id,
                          data.to_vertex_id,
                          next_data.to_vertex_id,
                          data.halfedge_id,
                          next_data.halfedge_id
                  }
                });
            }
            corner_dihedral_angle_average /= (float) vertex_data.size();
            corner_normal = glm::normalize(corner_normal);
            corner_face_center_average /= (float) vertex_data.size();
            // corner vertex
            corner_vertex_indices[vertex_id] = add_vertex_data_to_cell_data(
                    cell_data,
                    VertexType::CORNER,
                    corner_pos,
                    -corner_normal,
                    color,
                    corner_face_center_average,
                    zero,
                    corner_dihedral_angle_average
            );
            total_cell_vertex_count++;
        }
        for (auto& it : face_centers)
        {
            // center vertex
            const int halfface_id = it.first;
            const auto& center_pos = it.second;
            face_center_indices[halfface_id] = add_vertex_data_to_cell_data(
                    cell_data,
                    VertexType::CENTER,
                    center_pos,
                    -face_normals[halfface_id],
                    color,
                    zero,
                    zero,
                    0.0f
            );
            int num_halfface_vertices = 1;
            for (auto& attrib_data : halfface_vertices[halfface_id])
            {
                unsigned int face_vertex_index = add_vertex_data_to_cell_data(
                        cell_data,
                        attrib_data.type,
                        attrib_data.pos,
                        attrib_data.norm,
                        attrib_data.col,
                        attrib_data.face_center,
                        attrib_data.to_vertex,
                        attrib_data.dihedral_angle
                );
                halfface_vertices_indices[halfface_id].push_back({
                      face_vertex_index,
                      attrib_data.data.corner_vertex_id,
                      attrib_data.data.to_vertex_id,
                      attrib_data.data.next_to_vertex_id,
                      attrib_data.data.to_vertex_halfedge_id,
                      attrib_data.data.next_to_vertex_halfedge_id
                });
                corner_vertex_face_vertex_index[attrib_data.data.corner_vertex_id][halfface_id] = face_vertex_index;
                num_halfface_vertices++;
            }
            int offset = total_cell_vertex_count + m_vertex_offset_rounded;
            add_halfface_index_and_count(VAO::MESH_ROUNDED, halfface_id, offset, num_halfface_vertices);
            total_cell_vertex_count += num_halfface_vertices;
        }
        // reset to the start value of the cell, so we can count from the beginning again
        m_vertex_offset_rounded = cell_vertex_offset;
        add_cell_index_and_count(VAO::MESH_ROUNDED, cell.idx(), m_vertex_offset_rounded, total_cell_vertex_count);
        m_vertex_offset_rounded += total_cell_vertex_count;

        // our last job is to triangulate all of our vertices that we have created
        // we start by iterating over the vertices of our inner triangle. This way, we can find the closest corner
        // vertex and go on from there
        for (const auto& it : halfface_vertices_indices)
        {
            // this is the current halfface ovm id that we are in
            const int halfface_id = it.first;
            // index of the halfface center vertex
            const unsigned int face_center_index = face_center_indices[halfface_id];
            const auto& face_vertices_data = it.second;
            for (const auto& face_vertex : face_vertices_data)
            {
                // there is sadly no clean way to do this
                const int corner_vertex_id = face_vertex.corner_vertex_id;
                const int to_corner_vertex_id = face_vertex.to_vertex_id;
                const int next_to_corner_vertex_id = face_vertex.next_to_vertex_id;

                const int to_corner_vertex_halfedge_id = face_vertex.to_vertex_halfedge_id;
                const int next_to_corner_vertex_halfedge_id = face_vertex.next_to_vertex_halfedge_id;

                const int to_corner_vertex_edge_id = vOS::Mesh::edge_handle(OpenVolumeMesh::HalfEdgeHandle {face_vertex.to_vertex_halfedge_id}).idx();
                const int next_to_corner_vertex_edge_id = vOS::Mesh::edge_handle(OpenVolumeMesh::HalfEdgeHandle {face_vertex.next_to_vertex_halfedge_id}).idx();

                const unsigned int corner_vertex_index = corner_vertex_indices[corner_vertex_id];

                const unsigned int face_corner_vertex_index = face_vertex.index;
                const unsigned int face_to_corner_vertex_index = corner_vertex_face_vertex_index[to_corner_vertex_id][halfface_id];

                const unsigned int to_corner_vertex_halfedge_index = halfedge_vertex_indices[to_corner_vertex_edge_id][corner_vertex_id];
                const unsigned int next_to_corner_vertex_halfedge_index = halfedge_vertex_indices[next_to_corner_vertex_edge_id][corner_vertex_id];
                const unsigned int to_vertex_halfedge_index = halfedge_vertex_indices[to_corner_vertex_edge_id][to_corner_vertex_id];

                // face center
                add_cell_triangle_indices(cell_data, face_center_index, face_corner_vertex_index, face_to_corner_vertex_index);

                // corner triangle
                add_cell_triangle_indices(cell_data, corner_vertex_index, face_corner_vertex_index, next_to_corner_vertex_halfedge_index);
                add_cell_triangle_indices(cell_data, corner_vertex_index, to_corner_vertex_halfedge_index, face_corner_vertex_index);

                // triangles along the edge
                add_cell_triangle_indices(cell_data, to_corner_vertex_halfedge_index, face_to_corner_vertex_index, face_corner_vertex_index);
                add_cell_triangle_indices(cell_data, to_corner_vertex_halfedge_index, to_vertex_halfedge_index, face_to_corner_vertex_index);
            }
        }
        VecUtil::push_buffer(cell_data.indices, m_indices_rounded);
        add_attribute_data(VAO::MESH_ROUNDED, Attribute::POSITION, cell_data.vertex_positions);
        add_attribute_data(VAO::MESH_ROUNDED, Attribute::NORMAL, cell_data.vertex_normals);
        add_attribute_data(VAO::MESH_ROUNDED, Attribute::CELL_CENTER, cell_data.vertex_cell_centers);
        add_attribute_data(VAO::MESH_ROUNDED, Attribute::COLOR, cell_data.vertex_colors);
        add_attribute_data(VAO::MESH_ROUNDED, Attribute::PEEL_DEPTH, cell_data.vertex_peel_depths);
        add_attribute_data(VAO::MESH_ROUNDED, Attribute::IS_TRIANGLE, cell_data.vertex_is_triangle);
        add_attribute_data(VAO::MESH_ROUNDED, Attribute::IS_DIGGED, cell_data.vertex_is_digged);
        add_attribute_data(VAO::MESH_ROUNDED, Attribute::IS_ISOLATED, cell_data.vertex_is_isolated);
        add_attribute_data(VAO::MESH_ROUNDED, Attribute::VERTEX_TYPE, cell_data.vertex_types);
        add_attribute_data(VAO::MESH_ROUNDED, Attribute::FACE_CENTER, cell_data.face_center);
        add_attribute_data(VAO::MESH_ROUNDED, Attribute::TO_VERTEX, cell_data.to_vertex);
        add_attribute_data(VAO::MESH_ROUNDED, Attribute::DIHEDRAL_ANGLE, cell_data.dihedral_angle);
        add_attribute_data(VAO::MESH_ROUNDED, Attribute::SELECTION, cell_data.selection);
        add_attribute_data(VAO::MESH_ROUNDED, Attribute::HOVERED, cell_data.hovered);
        m_current_rounded_index += (int) cell_data.vertex_positions.size() / 3;
    }

    void MeshVertexBuffer::add_from_to_vertex(Mesh& mesh, const OpenVolumeMesh::VertexHandle& from,
                                              const OpenVolumeMesh::VertexHandle& to)
    {
        VecUtil::push_vec3(
                get_attrib_array(VAO::CYLINDER, Attribute::SELECTION_FROM_VERTEX),
                VecUtil::pos_to_vec3(mesh, from)
        );
        VecUtil::push_vec3(
                get_attrib_array(VAO::CYLINDER, Attribute::SELECTION_TO_VERTEX),
                VecUtil::pos_to_vec3(mesh, to)
        );
    }

    std::vector<float> MeshVertexBuffer::get_vertices(Mesh& mesh)
    {
        std::vector<float> vertices;
        vertices.reserve(mesh.n_vertices() * 3);
        for (auto v_it : mesh.vertices())
        {
            VecUtil::push_vec3(vertices, VecUtil::pos_to_vec3(mesh, v_it));
        }
        return vertices;
    }

    VertexArrayObject* MeshVertexBuffer::get_vao_by_face()
    {
        update_vertex_arrays();
        return m_vao.face;
    }

    VertexArrayObject* MeshVertexBuffer::get_vao_rounded()
    {
        update_vertex_arrays();
        return m_vao.rounded;
    }

    int MeshVertexBuffer::to_vertex_id(int value)
    {
        if (m_selection_map.vertex_ids.size() > value)
        {
            return m_selection_map.vertex_ids[value] + 1;
        }
        return 0;
    }

    int MeshVertexBuffer::to_edge_id(int value)
    {
        if (m_selection_map.edge_ids.size() > value)
        {
            return m_selection_map.edge_ids[value] + 1;
        }
        return 0;
    }

    int MeshVertexBuffer::to_halfface_id(int value)
    {
        if (m_selection_map.halfface_ids.size() > value)
        {
            return m_selection_map.halfface_ids[value] + 1;
        }
        return 0;
    }

    std::vector<float>& MeshVertexBuffer::get_original_vertices()
    {
        return m_original_vertices;
    }

    VertexArrayObject* MeshVertexBuffer::get_cylinder_vao()
    {
        update_vertex_arrays();
        return m_vao.selection_cylinder;
    }

    VertexArrayObject* MeshVertexBuffer::get_sphere_vao()
    {
        update_vertex_arrays();
        return m_vao.selection_sphere;
    }

    int MeshVertexBuffer::get_num_selection_vertices() const
    {
        return (int) m_selection_map.vertex_ids.size();
    }

    int MeshVertexBuffer::get_num_selection_edges() const
    {
        return (int) m_selection_map.edge_ids.size();
    }

    void MeshVertexBuffer::set_face_color(int face_id, float r, float g, float b, float a)
    {
        auto [hf0, hf1] = m_mesh.face_halffaces(OpenVolumeMesh::FaceHandle{face_id});
        glm::vec4 value = {r, g, b, a};
        if (hf0.is_valid())
        {
            update_halfface_attribute(VAO::MESH_FACE, Attribute::COLOR, hf0.idx(), value);
            update_halfface_attribute(VAO::MESH_ROUNDED, Attribute::COLOR, hf0.idx(), value);
        }
        if (hf1.is_valid())
        {
            update_halfface_attribute(VAO::MESH_FACE, Attribute::COLOR, hf1.idx(), value);
            update_halfface_attribute(VAO::MESH_ROUNDED, Attribute::COLOR, hf1.idx(), value);
        }
    }

    void MeshVertexBuffer::set_face_selection(int face_id, bool selected)
    {
        auto [hf0, hf1] = m_mesh.face_halffaces(OpenVolumeMesh::FaceHandle{face_id});
        float value = selected ? 1.0f : 0.0f;
        if (hf0.is_valid())
        {
            update_halfface_attribute(VAO::MESH_FACE, Attribute::SELECTION, hf0.idx(), value);
            update_halfface_attribute(VAO::MESH_ROUNDED, Attribute::SELECTION, hf0.idx(), value);
        }
        if (hf1.is_valid())
        {
            update_halfface_attribute(VAO::MESH_FACE, Attribute::SELECTION, hf1.idx(), value);
            update_halfface_attribute(VAO::MESH_ROUNDED, Attribute::SELECTION, hf1.idx(), value);
        }
    }

    void MeshVertexBuffer::set_halfface_color(int halfface_id, float r, float g, float b, float a)
    {
        auto hf = OpenVolumeMesh::HalfFaceHandle{halfface_id};
        glm::vec4 value = {r, g, b, a};
        if (hf.is_valid())
        {
            update_halfface_attribute(VAO::MESH_FACE, Attribute::COLOR, hf.idx(), value);
            update_halfface_attribute(VAO::MESH_ROUNDED, Attribute::COLOR, hf.idx(), value);
        }
    }

    glm::vec4 MeshVertexBuffer::get_halfface_color(int halfface_id)
    {
        return get_halfface_attribute<glm::vec4>(VAO::MESH_FACE, Attribute::COLOR, halfface_id);
    }

    void MeshVertexBuffer::set_halfface_selection(int halfface_id, bool selected)
    {
        auto hf = OpenVolumeMesh::HalfFaceHandle{halfface_id};
        float value = selected ? 1.0f : 0.0f;
        if (hf.is_valid())
        {
            update_halfface_attribute(VAO::MESH_FACE, Attribute::SELECTION, hf.idx(), value);
            update_halfface_attribute(VAO::MESH_ROUNDED, Attribute::SELECTION, hf.idx(), value);
        }
    }

    void MeshVertexBuffer::hover_halfface(int halfface_id)
    {
        if (m_current_hovered_halfface_id == halfface_id)
        {
            return;
        }
        if (halfface_id == -1)
        {
            reset_hover();
            return;
        }
        auto hf = OpenVolumeMesh::HalfFaceHandle{halfface_id};
        if (hf.is_valid())
        {
            reset_hover();
            update_halfface_attribute(VAO::MESH_FACE, Attribute::HOVERED, hf.idx(), 1.0f);
            update_halfface_attribute(VAO::MESH_ROUNDED, Attribute::HOVERED, hf.idx(), 1.0f);
            m_current_hovered_halfface_id = halfface_id;
        }
    }

    void MeshVertexBuffer::set_cell_color(int cell_id, float r, float g, float b, float a)
    {
        auto ch = OpenVolumeMesh::CellHandle {cell_id};
        glm::vec4 value = {r, g, b, a};
        if (ch.is_valid())
        {
            update_cell_attribute(VAO::MESH_FACE, Attribute::COLOR, ch.idx(), value);
            update_cell_attribute(VAO::MESH_ROUNDED, Attribute::COLOR, ch.idx(), value);
        }
    }

    glm::vec4 MeshVertexBuffer::get_cell_color(int cell_id)
    {
        return get_cell_attribute<glm::vec4>(VAO::MESH_FACE, Attribute::COLOR, cell_id);
    }

    void MeshVertexBuffer::set_cell_selection(int cell_id, bool selected)
    {
        auto ch = OpenVolumeMesh::CellHandle {cell_id};
        float value = selected ? 1.0f : 0.0f;
        if (ch.is_valid())
        {
            update_cell_attribute(VAO::MESH_FACE, Attribute::SELECTION, ch.idx(), value);
            update_cell_attribute(VAO::MESH_ROUNDED, Attribute::SELECTION, ch.idx(), value);
        }
    }

    void MeshVertexBuffer::hover_cell(int cell_id)
    {
        if (m_current_hovered_cell_id == cell_id)
        {
            return;
        }
        if (cell_id == -1)
        {
            reset_hover();
            return;
        }
        auto ch = OpenVolumeMesh::CellHandle{cell_id};
        if (ch.is_valid())
        {
            reset_hover();
            update_cell_attribute(VAO::MESH_FACE, Attribute::HOVERED, ch.idx(), 1.0f);
            update_cell_attribute(VAO::MESH_ROUNDED, Attribute::HOVERED, ch.idx(), 1.0f);
            m_current_hovered_cell_id = cell_id;
        }
    }

    void MeshVertexBuffer::reset_hover()
    {
        if (m_current_hovered_halfface_id != -1)
        {
            update_halfface_attribute(VAO::MESH_FACE, Attribute::HOVERED, m_current_hovered_halfface_id, 0.0f);
            update_halfface_attribute(VAO::MESH_ROUNDED, Attribute::HOVERED, m_current_hovered_halfface_id, 0.0f);
            m_current_hovered_halfface_id = -1;
        }
        if (m_current_hovered_cell_id != -1)
        {
            update_cell_attribute(VAO::MESH_FACE, Attribute::HOVERED, m_current_hovered_cell_id, 0.0f);
            update_cell_attribute(VAO::MESH_ROUNDED, Attribute::HOVERED, m_current_hovered_cell_id, 0.0f);
            m_current_hovered_cell_id = -1;
        }
    }

    void MeshVertexBuffer::set_cell_digged(int cell_id, bool digged)
    {
        auto cell = OpenVolumeMesh::CellHandle{cell_id};
        float value = digged ? 1.0f : 0.0f;
        if (cell.is_valid())
        {
            update_cell_attribute(VAO::MESH_FACE, Attribute::IS_DIGGED, cell.idx(), value);
            update_cell_attribute(VAO::MESH_ROUNDED, Attribute::IS_DIGGED, cell.idx(), value);
            update_cell_attribute(VAO::SPHERE, Attribute::IS_DIGGED, cell.idx(), value);
            update_cell_attribute(VAO::CYLINDER, Attribute::IS_DIGGED, cell.idx(), value);
        }
    }

    void MeshVertexBuffer::reset_digging()
    {
        float value = 0.0f;
        update_attribute(VAO::MESH_FACE, Attribute::IS_DIGGED, value);
        update_attribute(VAO::MESH_ROUNDED, Attribute::IS_DIGGED, value);
        update_attribute(VAO::SPHERE, Attribute::IS_DIGGED, value);
        update_attribute(VAO::CYLINDER, Attribute::IS_DIGGED, value);
    }


    void MeshVertexBuffer::set_cell_isolated(int cell_id)
    {
        if (cell_id == m_current_isolated_cell_id)
        {
            reset_isolation();
            m_current_isolated_cell_id = -1;
            return;
        }
        auto cell = OpenVolumeMesh::CellHandle{cell_id};
        if (cell.is_valid())
        {
            float invisible = 1.0f;
            float visible = 0.0f;
            update_attribute(VAO::MESH_FACE, Attribute::IS_ISOLATED, invisible);
            update_attribute(VAO::MESH_ROUNDED, Attribute::IS_ISOLATED, invisible);
            update_attribute(VAO::SPHERE, Attribute::IS_ISOLATED, invisible);
            update_attribute(VAO::CYLINDER, Attribute::IS_ISOLATED, invisible);
            update_cell_attribute(VAO::MESH_FACE, Attribute::IS_ISOLATED, cell.idx(), visible);
            update_cell_attribute(VAO::MESH_ROUNDED, Attribute::IS_ISOLATED, cell.idx(), visible);
            update_cell_attribute(VAO::SPHERE, Attribute::IS_ISOLATED, cell.idx(), visible);
            update_cell_attribute(VAO::CYLINDER, Attribute::IS_ISOLATED, cell.idx(), visible);
            m_current_isolated_cell_id = cell.idx();
        }
    }

    void MeshVertexBuffer::reset_isolation()
    {
        float value = 0.0f;
        update_attribute(VAO::MESH_FACE, Attribute::IS_ISOLATED, value);
        update_attribute(VAO::MESH_ROUNDED, Attribute::IS_ISOLATED, value);
        update_attribute(VAO::SPHERE, Attribute::IS_ISOLATED, value);
        update_attribute(VAO::CYLINDER, Attribute::IS_ISOLATED, value);
    }

    float MeshVertexBuffer::get_average_cell_size() const
    {
        return m_average_cell_size;
    }

    VertexArrayObject* MeshVertexBuffer::get_vertex_only_vao()
    {
        update_vertex_arrays();
        return m_vao.vertex_only;
    }

    glm::vec3 MeshVertexBuffer::normal_to_vec3(int halfface_id)
    {
        auto normal = m_normals[OpenVolumeMesh::HalfFaceHandle{halfface_id}];
        return {std::isnan(normal[0]) ? 0.0 : normal[0], std::isnan(normal[1]) ? 0.0 : normal[1], std::isnan(normal[2]) ? 0.0 : normal[2]};
    }

    void MeshVertexBuffer::update_vertex_arrays()
    {
        if (!is_loading_finished() || m_vao_update_data.empty())
        {
            return;
        }
        for (auto& data : m_vao_update_data)
        {
            int offset = -1;
            int count = -1;
            if (data.halfface_id != -1)
            {
                auto offset_count = get_halfface_index_and_count(data.vao, data.halfface_id);
                offset = offset_count.first;
                count = offset_count.second;
            }
            else if (data.cell_id != -1)
            {
                auto offset_count = get_cell_index_and_count(data.vao, data.cell_id);
                offset = offset_count.first;
                count = offset_count.second;
            }
            auto& vao_definitions = s_attribute_definitions.of(data.vao);
            auto& attrib_definition = vao_definitions[static_cast<int>(data.attribute)];
            auto& attrib_array = get_attrib_array(data.vao, data.attribute);
            int size = count * attrib_definition.element_count;
            int buffer_index = offset * attrib_definition.element_count;
            for (auto* vao : m_vertex_arrays[static_cast<int>(data.vao)])
            {
                if (data.fill_all)
                {
                    size_t total_indices = attrib_array.size() / attrib_definition.element_count;
                    for (size_t i = 0; i < total_indices; i++)
                    {
                        size_t index = i * attrib_definition.element_count;
                        set_attribute_buffer(attrib_array, index, data.value_size, data.value);
                    }
                    vao->update_attribute(attrib_array, attrib_definition.location);
                }
                else
                {
                    if (offset < 0 || count < 0)
                    {
                        break;
                    }
                    for (size_t i = 0; i < count; i++)
                    {
                        size_t index = (offset + i) * attrib_definition.element_count;
                        set_attribute_buffer(attrib_array, index, data.value_size, data.value);
                    }
                    vao->update_attribute(attrib_array, attrib_definition.location, buffer_index, size);
                }
            }
        }
        m_vao_update_data.clear();
    }

    template<typename T>
    void MeshVertexBuffer::update_halfface_attribute(VAO vao_id, Attribute attribute, int halfface_id, T data)
    {
        update_attribute(vao_id, attribute,data, halfface_id, -1);
    }

    template<typename T>
    void MeshVertexBuffer::update_cell_attribute(VAO vao_id, Attribute attribute, int cell_id, T data)
    {
        update_attribute(vao_id, attribute, data, -1, cell_id);
    }

    template<typename T>
    void MeshVertexBuffer::update_attribute(VAO vao_id, Attribute attribute, T value, int halfface_id, int cell_id)
    {
        auto [val, val_size] = get_value_and_size(value);
        m_vao_update_data.push_back({
                vao_id,
                attribute,
                val,
                val_size,
                halfface_id,
                cell_id,
                false
        });
    }

    template<typename T>
    void MeshVertexBuffer::update_attribute(VAO vao_id, Attribute attribute, T value)
    {
        auto [val, val_size] = get_value_and_size(value);
        m_vao_update_data.push_back({
                vao_id,
                attribute,
                val,
                val_size,
                -1,
                -1,
                true
        });
    }

    void MeshVertexBuffer::set_attribute_buffer(std::vector<float>& buffer, size_t offset, int value_size, const glm::vec4& value)
    {
        for (size_t i = 0; i < value_size; i++)
        {
            buffer[offset + i] = value[static_cast<int>(i)];
        }
    }

    template<typename T>
    std::pair<glm::vec4, int> MeshVertexBuffer::get_value_and_size(T value)
    {
        if constexpr(std::is_same_v<T, float>)
        {
            auto val = static_cast<float>(value);
            return std::make_pair(glm::vec4{val, 0.0f, 0.0f, 0.0f}, 1);
        }
        else if constexpr(std::is_same_v<T, glm::vec2>)
        {
            auto val = static_cast<glm::vec2>(value);
            return std::make_pair(glm::vec4{val.x, val.y, 0.0f, 0.0f}, 2);
        }
        else if constexpr(std::is_same_v<T, glm::vec3>)
        {
            auto val = static_cast<glm::vec3>(value);
            return std::make_pair(glm::vec4{val.x, val.y, val.z, 0.0f}, 3);
        }
        else if constexpr(std::is_same_v<T, glm::vec4>)
        {
            auto val = static_cast<glm::vec4>(value);
            return std::make_pair(glm::vec4{val.x, val.y, val.z, val.w}, 4);
        }
    }

    template<typename T>
    T MeshVertexBuffer::get_halfface_attribute(VAO vao_id, Attribute attribute, int halfface_id)
    {
        auto hf = OpenVolumeMesh::HalfFaceHandle{halfface_id};
        if (!is_loading_finished() || !hf.is_valid())
        {
            return get_default_value<T>();
        }
        auto [offset, count] = get_halfface_index_and_count(vao_id, halfface_id);
        return get_value_for_offset<T>(vao_id, attribute, offset);
    }

    template<typename T>
    T MeshVertexBuffer::get_cell_attribute(VAO vao_id, Attribute attribute, int cell_id)
    {
        auto cell = OpenVolumeMesh::CellHandle{cell_id};
        if (!is_loading_finished() || !cell.is_valid())
        {
            return get_default_value<T>();
        }
        auto [offset, count] = get_cell_index_and_count(vao_id, cell_id);
        return get_value_for_offset<T>(vao_id, attribute, offset);
    }

    template<typename T>
    T MeshVertexBuffer::get_default_value()
    {
        if constexpr(std::is_same_v<T, float>)
        {
            return 0;
        }
        else if constexpr(std::is_same_v<T, glm::vec2>)
        {
            return glm::vec2(0.0f, 0.0f);
        }
        else if constexpr(std::is_same_v<T, glm::vec3>)
        {
            return glm::vec3(0.0f, 0.0f, 0.0f);
        }
        else if constexpr(std::is_same_v<T, glm::vec4>)
        {
            return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        }
    }

    template<typename T>
    T MeshVertexBuffer::get_value_for_offset(VAO vao_id, Attribute attribute, int offset)
    {
        auto& vao_definitions = s_attribute_definitions.of(vao_id);
        auto& attrib_definition = vao_definitions[static_cast<int>(attribute)];
        auto& attrib_array = get_attrib_array(vao_id, attribute);
        size_t index = offset * attrib_definition.element_count;
        if constexpr(std::is_same_v<T, float>)
        {
            return attrib_array[index];
        }
        else if constexpr(std::is_same_v<T, glm::vec2>)
        {
            return glm::vec2{attrib_array[index], attrib_array[index + 1]};
        }
        else if constexpr(std::is_same_v<T, glm::vec3>)
        {
            return glm::vec3{attrib_array[index], attrib_array[index + 1], attrib_array[index + 2]};
        }
        else if constexpr(std::is_same_v<T, glm::vec4>)
        {
            return glm::vec4{attrib_array[index], attrib_array[index + 1], attrib_array[index + 2], attrib_array[index + 3]};
        }
        return get_default_value<T>();
    }

    template void MeshVertexBuffer::update_cell_attribute<float>(VAO, Attribute, int, float);
    template void MeshVertexBuffer::update_cell_attribute<glm::vec2>(VAO, Attribute, int, glm::vec2);
    template void MeshVertexBuffer::update_cell_attribute<glm::vec3>(VAO, Attribute, int, glm::vec3);
    template void MeshVertexBuffer::update_cell_attribute<glm::vec4>(VAO, Attribute, int, glm::vec4);

    template void MeshVertexBuffer::update_halfface_attribute<float>(VAO, Attribute, int, float);
    template void MeshVertexBuffer::update_halfface_attribute<glm::vec2>(VAO, Attribute, int, glm::vec2);
    template void MeshVertexBuffer::update_halfface_attribute<glm::vec3>(VAO, Attribute, int, glm::vec3);
    template void MeshVertexBuffer::update_halfface_attribute<glm::vec4>(VAO, Attribute, int, glm::vec4);

    template void MeshVertexBuffer::update_attribute<float>(VAO, Attribute, float, int, int);
    template void MeshVertexBuffer::update_attribute<glm::vec2>(VAO, Attribute, glm::vec2, int, int);
    template void MeshVertexBuffer::update_attribute<glm::vec3>(VAO, Attribute, glm::vec3, int, int);
    template void MeshVertexBuffer::update_attribute<glm::vec4>(VAO, Attribute, glm::vec4, int, int);

    template void MeshVertexBuffer::update_attribute<float>(VAO, Attribute, float);
    template void MeshVertexBuffer::update_attribute<glm::vec2>(VAO, Attribute, glm::vec2);
    template void MeshVertexBuffer::update_attribute<glm::vec3>(VAO, Attribute, glm::vec3);
    template void MeshVertexBuffer::update_attribute<glm::vec4>(VAO, Attribute, glm::vec4);

    template std::pair<glm::vec4, int> MeshVertexBuffer::get_value_and_size<float>(float);
    template std::pair<glm::vec4, int> MeshVertexBuffer::get_value_and_size<glm::vec2>(glm::vec2);
    template std::pair<glm::vec4, int> MeshVertexBuffer::get_value_and_size<glm::vec3>(glm::vec3);
    template std::pair<glm::vec4, int> MeshVertexBuffer::get_value_and_size<glm::vec4>(glm::vec4);

    template float MeshVertexBuffer::get_halfface_attribute(VAO, Attribute, int);
    template glm::vec2 MeshVertexBuffer::get_halfface_attribute(VAO, Attribute, int);
    template glm::vec3 MeshVertexBuffer::get_halfface_attribute(VAO, Attribute, int);
    template glm::vec4 MeshVertexBuffer::get_halfface_attribute(VAO, Attribute, int);

    template float MeshVertexBuffer::get_cell_attribute(VAO, Attribute, int);
    template glm::vec2 MeshVertexBuffer::get_cell_attribute(VAO, Attribute, int);
    template glm::vec3 MeshVertexBuffer::get_cell_attribute(VAO, Attribute, int);
    template glm::vec4 MeshVertexBuffer::get_cell_attribute(VAO, Attribute, int);

    template float MeshVertexBuffer::get_default_value();
    template glm::vec2 MeshVertexBuffer::get_default_value();
    template glm::vec3 MeshVertexBuffer::get_default_value();
    template glm::vec4 MeshVertexBuffer::get_default_value();

    template float MeshVertexBuffer::get_value_for_offset(VAO, Attribute, int);
    template glm::vec2 MeshVertexBuffer::get_value_for_offset(VAO, Attribute, int);
    template glm::vec3 MeshVertexBuffer::get_value_for_offset(VAO, Attribute, int);
    template glm::vec4 MeshVertexBuffer::get_value_for_offset(VAO, Attribute, int);
}
