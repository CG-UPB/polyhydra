
#include "MeshVertexBuffer.h"
#include "../rendering/meshes/CommonMeshes.h"

#include <OpenVolumeMesh/Attribs/NormalAttrib.hh>
#include <unordered_set>
#include "../util/VecUtil.h"

namespace vOS
{

    MeshVertexBuffer::MeshVertexBuffer(Mesh* mesh)
    {
        m_original_vertices = get_vertices(*mesh);
        generate_buffer(*mesh);

        m_vao_by_face = new VertexArrayObject(m_positions_by_face, m_indices);
        m_vao_by_face->add_attribute(m_normals_by_face, 1, 3);
        m_vao_by_face->add_attribute(m_cell_centers_by_face, 2, 3);
        m_vao_by_face->add_attribute(m_peel_depths_by_face, 3, 1);
        m_vao_by_face->add_attribute(m_is_digged_by_face, 4, 1);
        m_vao_by_face->add_attribute(m_colors_by_face, 5, 4);
        m_vao_by_face->add_attribute(m_is_isolated_by_face, 6, 1);
        m_vao_by_face->add_attribute(m_is_triangle_by_face, 7, 1);
        m_vao_by_face->add_attribute(m_selections, 10, 1);

        m_vao_rounded = new VertexArrayObject(m_positions_rounded, m_indices_rounded);
        m_vao_rounded->add_attribute(m_normals_rounded, 1, 3);
        m_vao_rounded->add_attribute(m_cell_centers_rounded, 2, 3);
        m_vao_rounded->add_attribute(m_peel_depths_rounded, 3, 1);
        m_vao_rounded->add_attribute(m_is_digged_rounded, 4, 1);
        m_vao_rounded->add_attribute(m_colors_rounded, 5, 4);
        m_vao_rounded->add_attribute(m_is_isolated_rounded, 6, 1);
        m_vao_rounded->add_attribute(m_is_triangle_rounded, 7, 1);
        m_vao_rounded->add_attribute(m_vertex_types_rounded, 8, 1);
        m_vao_rounded->add_attribute(m_face_center_or_to_vertex_rounded, 9, 4);

        m_vao_transparent_by_face = new VertexArrayObject(m_positions_by_face, m_indices);
        m_vao_transparent_by_face->add_attribute(m_normals_by_face, 1, 3);
        m_vao_transparent_by_face->add_attribute(m_cell_centers_by_face, 2, 3);
        m_vao_transparent_by_face->add_attribute(m_peel_depths_by_face, 3, 1);
        m_vao_transparent_by_face->add_attribute(m_is_digged_by_face, 4, 1);
        m_vao_transparent_by_face->add_attribute(m_colors_by_face, 5, 4);
        m_vao_transparent_by_face->add_attribute(m_is_isolated_by_face, 6, 1);
        m_vao_transparent_by_face->add_attribute(m_is_triangle_by_face, 7, 1);
        m_vao_transparent_by_face->add_attribute(m_selections, 10, 1);

        m_vao_transparent_rounded = new VertexArrayObject(m_positions_rounded, m_indices_rounded);
        m_vao_transparent_rounded->add_attribute(m_normals_rounded, 1, 3);
        m_vao_transparent_rounded->add_attribute(m_cell_centers_rounded, 2, 3);
        m_vao_transparent_rounded->add_attribute(m_peel_depths_rounded, 3, 1);
        m_vao_transparent_rounded->add_attribute(m_is_digged_rounded, 4, 1);
        m_vao_transparent_rounded->add_attribute(m_colors_rounded, 5, 4);
        m_vao_transparent_rounded->add_attribute(m_is_isolated_rounded, 6, 1);
        m_vao_transparent_rounded->add_attribute(m_is_triangle_rounded, 7, 1);
        m_vao_transparent_rounded->add_attribute(m_vertex_types_rounded, 8, 1);
        m_vao_transparent_rounded->add_attribute(m_face_center_or_to_vertex_rounded, 9, 4);

        m_sphere_vao = new VertexArrayObject(CommonMeshes::Sphere::selection_sphere().vertices(),
                                             CommonMeshes::Sphere::selection_sphere().indices());

        m_sphere_vao->add_attribute(m_normals_by_face, 1, 3, true);
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
        delete m_vao_by_face;
        delete m_vao_rounded;
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
            add_cell_by_faces(mesh, c_it);
            add_cell_rounded(mesh, c_it);
        }
        m_average_cell_size /= (float) mesh.n_cells();
    }

    unsigned int MeshVertexBuffer::add_vertex_data_to_cell_data(RoundedCellData& data, const float type, const glm::vec3& pos, const glm::vec3& norm, const glm::vec4& col, const glm::vec3& fc_or_tv, float angle)
    {
        auto& cell_center = m_cell_centers[data.cell_id];
        auto peel_depth = m_peel_depths[data.cell_id];
        unsigned int index = data.vertex_types.size();
        data.vertex_types.push_back(type);
        data.vertex_positions.push_back(pos.x);
        data.vertex_positions.push_back(pos.y);
        data.vertex_positions.push_back(pos.z);
        data.vertex_normals.push_back(norm.x);
        data.vertex_normals.push_back(norm.y);
        data.vertex_normals.push_back(norm.z);
        data.vertex_cell_centers.push_back(cell_center.x);
        data.vertex_cell_centers.push_back(cell_center.y);
        data.vertex_cell_centers.push_back(cell_center.z);
        data.vertex_colors.push_back(col.x);
        data.vertex_colors.push_back(col.y);
        data.vertex_colors.push_back(col.z);
        data.vertex_colors.push_back(col.w);
        data.vertex_peel_depths.push_back(peel_depth);
        data.vertex_is_triangle.push_back(1.0f);
        data.vertex_is_digged.push_back(1.0f);
        data.vertex_is_isolated.push_back(1.0f);
        data.face_center_or_to_vertex.push_back(fc_or_tv.x);
        data.face_center_or_to_vertex.push_back(fc_or_tv.y);
        data.face_center_or_to_vertex.push_back(fc_or_tv.z);
        data.face_center_or_to_vertex.push_back(angle);
        return index;
    }

    void MeshVertexBuffer::add_cell_triangle_indices(RoundedCellData& data, unsigned int i0, unsigned int i1, unsigned int i2)
    {
        data.indices.push_back(m_current_rounded_index + i0);
        data.indices.push_back(m_current_rounded_index + i2);
        data.indices.push_back(m_current_rounded_index + i1);
    }

    void MeshVertexBuffer::add_cell_rounded(Mesh& mesh, Cell cell)
    {
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

        RoundedCellData cell_data;
        cell_data.cell_id = cell.idx();
        glm::vec4 color(1.0f, 1.0f, 1.0f, 0.0f);
        glm::vec3 zero(0.0f);
        // iterate over the vertices that we have collected earlier
        for (auto& it: cell_vertices)
        {
            // ovm id of the corner vertex
            const int vertex_id = it.first;
            // the outgoing halfedges and halffaces in a circle around the corner
            const auto& vertex_data = it.second;
            glm::vec3 corner_normal(0.0f);
            glm::vec3 corner_pos = VecUtil::pos_to_vec3(mesh, vertex_id);
            glm::vec3 corner_move_dir(0.0f);
            float corner_angle = 0.0f;
            for (size_t i = 0; i < vertex_data.size(); i++)
            {
                auto& prev_data = vertex_data[(i + 2) % vertex_data.size()];
                auto& data = vertex_data[(i + 1) % vertex_data.size()];
                auto& next_data = vertex_data[i];

                glm::vec3 prev_to_vertex_pos = VecUtil::pos_to_vec3(mesh, prev_data.to_vertex_id);
                glm::vec3 to_vertex_pos = VecUtil::pos_to_vec3(mesh, data.to_vertex_id);
                glm::vec3 next_to_vertex_pos = VecUtil::pos_to_vec3(mesh, next_data.to_vertex_id);

                float inner_angle = VecUtil::get_angle(to_vertex_pos - corner_pos, next_to_vertex_pos - corner_pos);
                float prev_inner_angle = VecUtil::get_angle(to_vertex_pos - corner_pos, prev_to_vertex_pos - corner_pos);
                glm::vec3 face_dir = glm::cross(to_vertex_pos - corner_pos, next_to_vertex_pos - corner_pos);

                corner_normal += face_dir;
                float corner_move_weight = M_PI * 2.0 - inner_angle - prev_inner_angle;
                corner_move_dir += glm::normalize(to_vertex_pos - corner_pos) * corner_move_weight;

                glm::vec3 face_normal = VecUtil::normal_to_vec3(mesh, data.halfface_id);
                glm::vec3 prev_face_normal = VecUtil::normal_to_vec3(mesh, prev_data.halfface_id);

                float dihedral_angle = M_PI - VecUtil::get_angle(face_normal, prev_face_normal);
                corner_angle += dihedral_angle;

                glm::vec3 edge_normal = glm::normalize(face_normal + prev_face_normal);
                auto edge = vOS::Mesh::edge_handle(OpenVolumeMesh::HalfEdgeHandle{data.halfedge_id});
                // edge vertex
                halfedge_vertex_indices[edge.idx()][data.from_vertex_id] = add_vertex_data_to_cell_data(
                        cell_data,
                        ROUNDED_VERTEX_TYPE_EDGE,
                        corner_pos,
                        edge_normal,
                        color,
                        to_vertex_pos,
                        dihedral_angle
                );
                // face vertex
                glm::vec3 face_center = face_centers[data.halfface_id];
                unsigned int face_vertex_index = add_vertex_data_to_cell_data(
                        cell_data,
                        ROUNDED_VERTEX_TYPE_FACE,
                        corner_pos,
                        face_normal,
                        color,
                        face_center,
                        0.0f
                );
                halfface_vertices_indices[data.halfface_id].push_back({
                    face_vertex_index,
                    vertex_id,
                    data.to_vertex_id,
                    next_data.to_vertex_id,
                    data.halfedge_id,
                    next_data.halfedge_id
                });
                corner_vertex_face_vertex_index[vertex_id][data.halfface_id] = face_vertex_index;
            }
            corner_angle /= (float) vertex_data.size();
            corner_normal = glm::normalize(corner_normal);
            corner_move_dir = glm::normalize(corner_move_dir);
            // corner vertex
            corner_vertex_indices[vertex_id] = add_vertex_data_to_cell_data(
                    cell_data,
                    ROUNDED_VERTEX_TYPE_CORNER,
                    corner_pos,
                    corner_normal,
                    color,
                    corner_move_dir,
                    corner_angle
            );
        }
        for (auto& it : face_centers)
        {
            // center vertex
            const int halfface_id = it.first;
            const auto& center_pos = it.second;
            face_center_indices[halfface_id] = add_vertex_data_to_cell_data(
                    cell_data,
                    ROUNDED_VERTEX_TYPE_CENTER,
                    center_pos,
                    face_normals[halfface_id],
                    color,
                    zero,
                    0.0f
            );
        }

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
                add_cell_triangle_indices(cell_data, to_corner_vertex_halfedge_index, next_to_corner_vertex_halfedge_index, corner_vertex_index);

                // opposite of corner triangle
                add_cell_triangle_indices(cell_data, to_corner_vertex_halfedge_index, face_corner_vertex_index, next_to_corner_vertex_halfedge_index);

                add_cell_triangle_indices(cell_data, to_corner_vertex_halfedge_index, face_to_corner_vertex_index, face_corner_vertex_index);
                add_cell_triangle_indices(cell_data, to_corner_vertex_halfedge_index, to_vertex_halfedge_index, face_to_corner_vertex_index);
            }
        }
        // if you have a prettier way to write this tell me please
        m_indices_rounded.insert(m_indices_rounded.end(), cell_data.indices.begin(), cell_data.indices.end());
        m_positions_rounded.insert(m_positions_rounded.end(), cell_data.vertex_positions.begin(), cell_data.vertex_positions.end());
        m_normals_rounded.insert(m_normals_rounded.end(), cell_data.vertex_normals.begin(), cell_data.vertex_normals.end());
        m_cell_centers_rounded.insert(m_cell_centers_rounded.end(), cell_data.vertex_cell_centers.begin(), cell_data.vertex_cell_centers.end());
        m_colors_rounded.insert(m_colors_rounded.end(), cell_data.vertex_colors.begin(), cell_data.vertex_colors.end());
        m_peel_depths_rounded.insert(m_peel_depths_rounded.end(), cell_data.vertex_peel_depths.begin(), cell_data.vertex_peel_depths.end());
        m_is_triangle_rounded.insert(m_is_triangle_rounded.end(), cell_data.vertex_is_triangle.begin(), cell_data.vertex_is_triangle.end());
        m_is_digged_rounded.insert(m_is_digged_rounded.end(), cell_data.vertex_is_digged.begin(), cell_data.vertex_is_digged.end());
        m_is_isolated_rounded.insert(m_is_isolated_rounded.end(), cell_data.vertex_is_isolated.begin(), cell_data.vertex_is_isolated.end());
        m_vertex_types_rounded.insert(m_vertex_types_rounded.end(), cell_data.vertex_types.begin(), cell_data.vertex_types.end());
        m_face_center_or_to_vertex_rounded.insert(m_face_center_or_to_vertex_rounded.end(), cell_data.face_center_or_to_vertex.begin(), cell_data.face_center_or_to_vertex.end());

        m_current_rounded_index += (int) cell_data.vertex_positions.size() / 3;
    }

    void MeshVertexBuffer::add_cell_by_faces(Mesh& mesh, Cell cell)
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
        auto [min, max] = VecUtil::get_bounding_box(vertices);
        glm::vec3 diameter = max - min;
        float size = std::max(std::max(diameter.x, diameter.y), diameter.z);
        m_average_cell_size += size;
        glm::vec3 cell_center = min + (max - min) * 0.5f;
        m_cell_centers[cell.idx()] = cell_center;

        // get peel depth of the cell
        int peel_depth = peel_property[cell];
        m_peel_depths[cell.idx()] = (float) peel_depth;

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
                glm::vec3 barycenter = {0,0,0};
                // iterate over the halfedges of the halfface
                for (auto vertex_pos: original_face_vertices)
                {
                    // get geometry data
                    VertexData v_data;
                    v_data.position = vertex_pos;
                    barycenter += vertex_pos;
                    v_data.normal = -normal;
                    face_data.vertices.push_back(v_data);
                }

                // Add face data
                face_data.face_ids.push_back(face_id);
                add_face_indices(mesh, face_data);
                m_num_vertices += (int) original_face_vertices.size();

                m_face_centers.emplace(face_id, barycenter / 3.0f);
                m_face_normals.emplace(face_id, normal);
                faces.push_back(face_data);
            } else if (original_face_vertices.size() > 3)
            {
                bool first_edge = true;
                glm::vec3 previous_position(0.0f);
                glm::vec3 face_normal(0.0f);

                // Triangulate Face
                // Create a new vertex that is our midpoint
                face_data.vertices.push_back(VertexData{});

                // Get Midpoint of all Vertices
                glm::vec3 midpoint(0.0f);
                for (auto vertex_pos: original_face_vertices)
                {
                    midpoint += vertex_pos;
                }
                midpoint /= original_face_vertices.size();

                // Add every other Vertex to the list, and calculate the local normals for each
                for (auto vertex_pos: original_face_vertices)
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
                    } else
                    {
                        first_edge = false;
                        previous_position = vertex_pos;
                    }
                }
                face_normal /= -glm::length(face_normal);
                face_data.vertices[0].position = midpoint;
                face_data.vertices[0].normal = face_normal;

                m_face_centers.emplace(face_id, midpoint);
                m_face_normals.emplace(face_id, face_normal);

                // Add Vertex Data
                for (auto vertex_pos: original_face_vertices)
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
            } else
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

        m_start_of_cell_vertices[cell.idx()] = m_is_digged_by_face.size();
        for (const FaceData& face: faces)
        {
            float is_triangle = (face.vertices.size() > 3) ? 0.0f : 1.0f;

            // fill up vertex data
            for (const VertexData& vertex: face.vertices)
            {
                // position
                m_positions_by_face.push_back(vertex.position.x);
                m_positions_by_face.push_back(vertex.position.y);
                m_positions_by_face.push_back(vertex.position.z);

                // normal
                m_normals_by_face.push_back(vertex.normal.x);
                m_normals_by_face.push_back(vertex.normal.y);
                m_normals_by_face.push_back(vertex.normal.z);

                // cell center
                m_cell_centers_by_face.push_back(cell_center.x);
                m_cell_centers_by_face.push_back(cell_center.y);
                m_cell_centers_by_face.push_back(cell_center.z);

                // Color
                m_colors_by_face.push_back(1);
                m_colors_by_face.push_back(1);
                m_colors_by_face.push_back(1);
                m_colors_by_face.push_back(0);

                // Selection Status
                m_selections.push_back(vertex.normal.x > 0.5 ? 1 : 0);

                m_peel_depths_by_face.push_back((float) peel_depth);
                //std::cout << peel_property[cell] <<std::endl;

                m_is_digged_by_face.push_back(1.0f);
                m_is_isolated_by_face.push_back(1.0f);
                nbr_vertices_of_cell++;
                m_is_triangle_by_face.push_back(is_triangle);
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
        int offset_index = m_face_offset_array[buffer_index];
        int color_array_index = offset_index * 4;

        for (int i = 0; i < vertex_count; i++)
        {
            m_colors_by_face[color_array_index + (i * 4)] = r;
            m_colors_by_face[color_array_index + (i * 4) + 1] = g;
            m_colors_by_face[color_array_index + (i * 4) + 2] = b;
            m_colors_by_face[color_array_index + (i * 4) + 3] = a;
        }
        m_update_vao = true;
    }

    void MeshVertexBuffer::set_face_selection(int ovm_id, bool selected)
    {
        // Out of Bounce Check
        if(ovm_id < 0 || ovm_id > m_ovm_to_gl_face_indizes.size())
            return;

        int buffer_index = m_ovm_to_gl_face_indizes[ovm_id];
        int vertex_count = m_face_vertex_count[buffer_index];
        int offset_index = m_face_offset_array[buffer_index];

        for(int i = 0; i< vertex_count; i++)
        {
            m_selections[offset_index + i] = selected ? 1 : 0;
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

    VertexArrayObject* MeshVertexBuffer::get_vao_by_face()
    {
        if (m_update_vao)
        {
            m_vao_by_face->update_attribute(m_colors_by_face, 6);
            m_vao_by_face->update_attribute(m_selections, 10);
            m_update_vao = false;
        }
        return m_vao_by_face;
    }

    VertexArrayObject* MeshVertexBuffer::get_vao_rounded()
    {
        if (m_update_vao)
        {
            m_vao_rounded->update_attribute(m_colors_rounded, 6);
            m_update_vao = false;
        }
        return m_vao_rounded;
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
            m_is_digged_by_face[start + i] = newValue;
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

        m_vao_by_face->update_attribute(m_is_digged_by_face, 5);
        m_sphere_vao->update_attribute(m_sphere_is_digged, 5);
        m_cylinder_vao->update_attribute(m_cylinder_is_digged, 5);
    }

    void MeshVertexBuffer::reset_digging()
    {
        for (size_t i = 0; i < m_is_digged_by_face.size(); i++)
        {
            m_is_digged_by_face[i] = 1.0;
        }
        for (size_t i = 0; i < m_sphere_is_digged.size(); i++)
        {
            m_sphere_is_digged[i] = 1.0;
        }
        for (size_t i = 0; i < m_cylinder_is_digged.size(); i++)
        {
            m_cylinder_is_digged[i] = 1.0;
        }

        m_vao_by_face->update_attribute(m_is_digged_by_face, 5);
        m_sphere_vao->update_attribute(m_sphere_is_digged, 5);
        m_cylinder_vao->update_attribute(m_cylinder_is_digged, 5);
    }


    void MeshVertexBuffer::update_isolate_buffer(int id, float newValue)
    {
        int nbr_vertices = m_size_of_cell_vertices[id];
        int start = m_start_of_cell_vertices[id];

        for (size_t i = 0; i < nbr_vertices; i++)
        {
            m_is_isolated_by_face[start + i] = newValue;
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
        for (size_t i = 0; i < m_is_isolated_by_face.size(); i++)
        {
            m_is_isolated_by_face[i] = 0.0;
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
        m_vao_by_face->update_attribute(m_is_isolated_by_face, 7);
        m_sphere_vao->update_attribute(m_sphere_is_isolated, 6);
        m_cylinder_vao->update_attribute(m_cylinder_is_isolated, 6);
    }

    void MeshVertexBuffer::reset_isolation()
    {
        for (size_t i = 0; i < m_is_isolated_by_face.size(); i++)
        {
            m_is_isolated_by_face[i] = 1.0;
        }
        for (size_t i = 0; i < m_sphere_is_isolated.size(); i++)
        {
            m_sphere_is_isolated[i] = 1.0;
        }
        for (size_t i = 0; i < m_cylinder_is_isolated.size(); i++)
        {
            m_cylinder_is_isolated[i] = 1.0;
        }

        m_vao_by_face->update_attribute(m_is_isolated_by_face, 7);
        m_sphere_vao->update_attribute(m_sphere_is_isolated, 6);
        m_cylinder_vao->update_attribute(m_cylinder_is_isolated, 6);
    }

    float MeshVertexBuffer::get_average_cell_size() const
    {
        return m_average_cell_size;
    }

}
