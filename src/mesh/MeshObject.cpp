
#include "MeshObject.h"

#include "../panels/LogWindow.h"
#include "MeshProperties.h"
#include "mesh/MeshTextureBuffer.h"

namespace volumeshOS::Internal
{

    MeshObject::MeshObject(int id) : m_id(id)
    {
        // empty mesh
        m_mesh = std::make_shared<OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>>();
    }

    void MeshObject::set_mesh(const std::shared_ptr<OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>>& mesh)
    {
        // copy given mesh
        m_mesh = mesh;
        MeshProperties::setup_mesh_properties(*m_mesh);

        // calculates the depth of vertices and cells (saved in peel_property for cells)
        calculate_peel_depth();

        // calculates the amount of ids the mesh needs
        calculate_mesh_offset();

        translate(glm::vec3(0.0f));
        scale(glm::vec3(1.0f));

        m_mvb = std::make_shared<MeshVertexBuffer>(m_mesh);
        m_mtb = std::make_shared<MeshTextureBuffer>(m_mesh);
    }

    void MeshObject::update_vertex_buffer()
    {
        if (m_mvb != nullptr && !m_mvb->is_loading_finished())
        {
            int load_cells_per_frame = 42;
            for (size_t i = 0; i < load_cells_per_frame; i++)
            {
                m_mvb->load_next_cell();
            }
        }
    }

    void MeshObject::calculate_mesh_offset()
    {
        std::vector<glm::vec3> vertices;
        for (auto v_it: m_mesh->vertices())
        {
            vertices.push_back(VecUtil::pos_to_vec3(*m_mesh, v_it));
        }
        auto [min, max] = VecUtil::get_bounding_box(vertices);
        glm::vec3 diameter = max - min;
        m_data.position_offset = min + (diameter * 0.5f);
        // all meshes should have the same screen size, regardless of their actual size
        m_data.scale_normalization = 7.0f / std::max(std::max(diameter.x, diameter.y), diameter.z);
    }

    void MeshObject::calculate_peel_depth()
    {
        OpenVolumeMesh::CellPropertyT<int> cell_peel_property = m_mesh->request_cell_property<int>("PeelDepth");
        OpenVolumeMesh::VertexPropertyT<int> vertex_peel_property = m_mesh->request_vertex_property<int>("PeelDepth");

        std::vector<OpenVolumeMesh::VertexHandle> act_level;
        std::vector<OpenVolumeMesh::VertexHandle> next_level;

        // initialize every vertex on boundary with peel_level = 0, else: -1
        for (auto vertex: m_mesh->vertices())
        {
            if (m_mesh->has_vertex_bottom_up_incidences() && m_mesh->is_boundary(vertex))
            {
                vertex_peel_property[vertex] = 0;
                act_level.push_back(vertex);
            }
            else
            {
                vertex_peel_property[vertex] = -1;
            }
        }

        // actual depth
        int depth = 0;

        while (!act_level.empty())
        {
            depth++;
            // to get the next layer we get the neighbour vertices of the vertices of actual layer
            for (auto vertex: act_level)
            {
                // vertices of next layer are the vertices of adjacent cells (especially important for 90°+ angles)
                for (auto neighbour_cell: m_mesh->vertex_cells(vertex))
                {
                    for (auto neighbour: m_mesh->cell_vertices(neighbour_cell))
                    {
                        // if not yet initalized: peel_depth of vertex = actual_depth
                        if (vertex_peel_property[neighbour] == -1)
                        {
                            vertex_peel_property[neighbour] = depth;
                            // only fills with vertices that are not yet initialized
                            next_level.push_back(neighbour);
                        }
                    }
                }
            }
            // the next level is now the actual level
            act_level.clear();
            act_level.insert(act_level.begin(), next_level.begin(), next_level.end());
            next_level.clear();
        }

        // now evaluate peel_level of cells: minimum peel_level of all vertices of the cell
        int max_depth = 0;
        for (auto cell: m_mesh->cells())
        {
            //
            int minimum = -1;
            for (auto cell_vertex: m_mesh->cell_vertices(cell))
            {
                // the first one is already the minimum
                if (minimum == -1)
                {
                    minimum = vertex_peel_property[cell_vertex];
                }
                else if (vertex_peel_property[cell_vertex] < minimum)
                {
                    minimum = vertex_peel_property[cell_vertex];
                }
            }
            // update peel_depth of cell
            cell_peel_property[cell] = minimum;
            // update maximum depth found
            if (minimum > max_depth)
            {
                max_depth = minimum;
            }
        }

        m_data.max_peel_depth = max_depth;
    }

    void MeshObject::set_halfface_color(int ovm_id, const glm::vec4& color)
    {
        m_mvb->set_halfface_color(ovm_id, color.r, color.g, color.b, color.a);
    }

    void MeshObject::set_face_color(int ovm_id, const glm::vec4& color)
    {
        m_mvb->set_face_color(ovm_id, color.r, color.g, color.b, color.a);
    }

    void MeshObject::set_cell_color(int ovm_id, const glm::vec4& color)
    {
        m_mvb->set_cell_color(ovm_id, color.r, color.g, color.b, color.a);
    }

    void MeshObject::set_mesh_color(const glm::vec4& color)
    {
        m_data.color = color;
    }

    int MeshObject::to_vertex_id(int value)
    {
        return m_mvb->to_vertex_id(value);
    }

    int MeshObject::to_edge_id(int value)
    {
        return m_mvb->to_edge_id(value);
    }

    int MeshObject::to_halfface_id(int value)
    {
        return m_mvb->to_halfface_id(value);
    }

    std::shared_ptr<VertexArrayObject> MeshObject::get_vao() const
    {
        return m_mvb->get_vao_by_face();
    }

    std::shared_ptr<MeshTextureBuffer> MeshObject::get_mtb() const
    {
        return m_mtb;
    }

    std::shared_ptr<VertexArrayObject> MeshObject::get_sphere_vao() const
    {
        return m_mvb->get_sphere_vao();
    }

    std::shared_ptr<VertexArrayObject> MeshObject::get_cylinder_vao() const
    {
        return m_mvb->get_cylinder_vao();
    }

    std::shared_ptr<OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>> MeshObject::get_ovm() const
    {
        return m_mesh;
    }

    bool MeshObject::is_bezier_mesh() const
    {
        return *get_ovm()->request_mesh_property<bool>(MeshProperties::PROP_IS_BEZIER).begin();
    }

    int MeshObject::calculate_selection_size() const
    {
        int size = 0;
        for (auto hf_it: m_mesh->halffaces())
        {
            int num_halfface_vertices = 0;
            for (auto vhf_it: m_mesh->halfface_vertices(hf_it))
            {
                num_halfface_vertices++;
            }
            // number of halfface edges, if we put a vertex at the center and connect it to all other vertices
            size += num_halfface_vertices * 2;
        }
        return size;
    }

    int MeshObject::get_num_visible_vertices() const
    {
        return m_mvb->get_num_selection_vertices();
    }

    int MeshObject::get_num_visible_edges() const
    {
        return m_mvb->get_num_selection_edges();
    }

    void MeshObject::set_selection_offset(int start)
    {
        m_selection_offset = {start, start + calculate_selection_size()};
    }

    std::pair<glm::vec3, glm::vec3>& MeshObject::get_world_bb(const glm::mat4& transform)
    {
        if (m_data.slice_locked)
        {
            return m_transformed_bb;
        }

        std::vector<float> vertices;
        for (auto v_it: m_mesh->vertices())
        {
            auto v_pos = m_mesh->vertex(v_it);
            glm::vec4 vec(v_pos[0], v_pos[1], v_pos[2], 1.0);
            // apply transformation matrix
            vec = transform * vec;

            vertices.push_back(vec[0]);
            vertices.push_back(vec[1]);
            vertices.push_back(vec[2]);
        }

        glm::vec4 min(vertices[0], vertices[1], vertices[2], 1.0);
        glm::vec4 max(vertices[0], vertices[1], vertices[2], 1.0);
        for (int i = 0; i < vertices.size(); i += 3)
        {
            glm::vec3 vertex(vertices[i], vertices[i + 1], vertices[i + 2]);
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

        // undo the transformation for bounding box
        glm::vec3 m1(glm::inverse(transform) * min);
        glm::vec3 m2(glm::inverse(transform) * max);
        m_transformed_bb = std::make_pair(m1, m2);
        return m_transformed_bb;
    }


    glm::vec3& MeshObject::get_slice_dir(const glm::mat4& world_transform, const glm::vec3& view_dir)
    {
        if (!m_data.slice_locked)
        {
            m_just_locked = true;
            m_slice_dir = glm::vec3(glm::inverse(world_transform) * glm::vec4(view_dir, 0.0f));
        }
        else
        {
            if (m_just_locked)
            {
                m_just_locked = false;
                m_slice_dir = glm::vec3(glm::inverse(m_data.get_transform()) * glm::vec4(view_dir, 0.0f));
            }
        }
        return m_slice_dir;
    }

    float MeshObject::get_max_peel_depth() const
    {
        return m_data.max_peel_depth;
    }

    std::shared_ptr<MeshVertexBuffer> MeshObject::get_mvb() const
    {
        return m_mvb;
    }

    glm::vec3 MeshObject::get_min()
    {
        return m_mvb->get_min_bounding_box();
    }

    glm::vec3 MeshObject::get_max()
    {
        return m_mvb->get_max_bounding_box();
    }

    void MeshObject::translate(const glm::vec3& vec)
    {
        m_data.position = vec;
        m_data.update_transform();
    }

    void MeshObject::scale(const glm::vec3& vec)
    {
        m_data.scale = vec;
        m_data.update_transform();
    }

    void MeshObject::rotate(float angle, const glm::vec3& axis)
    {
        m_data.rotation = glm::rotate(m_data.rotation, angle, glm::normalize(axis));
        m_data.update_transform();
    }

    void MeshObject::reset_rotation()
    {
        m_data.rotation = glm::mat4(1.0f);
        m_data.update_transform();
    }

    void MeshData::update_transform()
    {
        auto scaling = glm::scale(glm::mat4(1.0f), scale * scale_normalization);
        auto translation = glm::translate(glm::mat4(1.0f),  position - position_offset);
        glm::mat4 rot = glm::translate(glm::mat4(1.0), position) * rotation;
        glm::mat4 scl = scaling * glm::translate(glm::mat4(1.0), -position);
        transformation = rot * scl * translation;
    }

}

