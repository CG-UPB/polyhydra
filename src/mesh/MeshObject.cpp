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
#include "../rendering/shapes/Sphere.h"
#include "../rendering/shapes/Cylinder.h"

namespace vOS
{

    MeshObject::MeshObject()
    {
        m_mesh = new OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f>();
        m_should_update = false;

        OpenVolumeMesh::VertexPropertyT<bool> highlightProp = m_mesh->request_vertex_property<bool>("VertexHighlight");
        highlightProp->set_persistent(true);
        OpenVolumeMesh::VertexPropertyT <OpenVolumeMesh::Vec3f> highlightColProp = m_mesh->request_vertex_property<OpenVolumeMesh::Vec3f>(
                "VertexHighlightColor");
        highlightColProp->set_persistent(true);

    }

    MeshObject::MeshObject(OpenVolumeMesh::GeometryKernel <OpenVolumeMesh::Vec3f>* mesh) : MeshObject()
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

    void MeshObject::select_element(int id, int type){
        int shape_key = type * 114748364 + id;

        // We can't select an element twice
        bool already_selected = element_is_selected(id,type);
        if(already_selected)
            return;

        if(type == 0) {
            m_selected_faces.insert(id);

            // Add Shape

            OpenVolumeMesh::FaceHandle face(id);

            auto pick_pos = m_mesh->barycenter(face);
            auto* shape = new Cylinder();
            shape->set_scale(0.02f, 0.02f, 0.02f);
            shape->set_position(pick_pos[0], pick_pos[1], pick_pos[2]);
            shape->set_base_color(1.0f, 0.0f, 0.0f);

            // There a guaranteed Mutex Guard around this method
            Window::instance().rendering_mutex.unlock();
            int shape_id = Window::instance().add_shape(shape);
            Window::instance().rendering_mutex.lock();

            m_created_shapes.insert({shape_key, shape_id});
        }else if(type == 1) {
            m_selected_vertices.insert(id);

            // Add Shape

            OpenVolumeMesh::VertexHandle vertex(id);

            auto pick_pos = m_mesh->vertex(vertex);
            auto* shape = new Sphere();
            shape->set_scale(0.02f, 0.02f, 0.02f);
            shape->set_position(pick_pos[0], pick_pos[1], pick_pos[2]);
            shape->set_base_color(0.0f, 1.0f, 0.0f);

            // There a guaranteed Mutex Guard around this method
            Window::instance().rendering_mutex.unlock();
            int shape_id = Window::instance().add_shape(shape);
            Window::instance().rendering_mutex.lock();

            m_created_shapes.insert({shape_key, shape_id});
        }else if(type == 2){
            m_selected_edges.insert(id);

            // Add Shape
            OpenVolumeMesh::EdgeHandle edge(id);
            auto vertices = m_mesh->edge_vertices(edge);
            auto v0 = m_mesh->vertex(vertices[0]);
            auto v1 = m_mesh->vertex(vertices[1]);
            auto pick_pos = glm::vec3(v0[0] + (v1[0] - v0[0]) * 0.5, v0[1] + (v1[1] - v0[1]) * 0.5, v0[2] + (v1[2] - v0[2]) * 0.5);
            auto* shape = new Box();
            shape->set_scale(0.02f, 0.02f, 0.02f);
            shape->set_position(pick_pos[0], pick_pos[1], pick_pos[2]);
            shape->set_base_color(0.0f, 0.0f, 1.0f);

            // There a guaranteed Mutex Guard around this method
            Window::instance().rendering_mutex.unlock();
            int shape_id = Window::instance().add_shape(shape);
            Window::instance().rendering_mutex.lock();

            m_created_shapes.insert({shape_key, shape_id});
        }else {
            m_selected_cells.insert(id);

        }
    }

    void MeshObject::unselect_all(){
        // Delete Face Elements
        for(int element : m_selected_faces)
        {
            // Delete Shape Element
            int shape_key = 0 * 114748364 + element;
            int shape_id = m_created_shapes[shape_key];

            Window::instance().rendering_mutex.unlock();
            Window::instance().remove_shape(shape_id);
            Window::instance().rendering_mutex.lock();
        }
        m_selected_faces.clear();

        // Delete Vertex Elements
        for(int element : m_selected_vertices)
        {
            // Delete Shape Element
            int shape_key = 1 * 114748364 + element;
            int shape_id = m_created_shapes[shape_key];

            Window::instance().rendering_mutex.unlock();
            Window::instance().remove_shape(shape_id);
            Window::instance().rendering_mutex.lock();
        }
        m_selected_vertices.clear();
        // Delete Edge Elements
        for(int element : m_selected_edges)
        {
            // Delete Shape Element
            int shape_key = 2 * 114748364 + element;
            int shape_id = m_created_shapes[shape_key];

            Window::instance().rendering_mutex.unlock();
            Window::instance().remove_shape(shape_id);
            Window::instance().rendering_mutex.lock();
        }
        m_selected_edges.clear();
        // Delete Face Elements
        for(int element : m_selected_cells)
        {
            // Delete Shape Element
            int shape_key = 3 * 114748364 + element;
            int shape_id = m_created_shapes[shape_key];

            Window::instance().rendering_mutex.unlock();
            Window::instance().remove_shape(shape_id);
            Window::instance().rendering_mutex.lock();
        }
        m_selected_cells.clear();
    }

    void MeshObject::unselect_element(int id, int type){
        // Element must be selected to be unselectable
        bool is_selected = element_is_selected(id,type);
        if(!is_selected)
            return;

        if(type == 0) {

            auto entry = m_selected_faces.find(id);
            m_selected_faces.erase(entry);
        }else if(type == 1) {
            auto entry = m_selected_vertices.find(id);
            m_selected_vertices.erase(entry);
        }else if(type == 2) {
            auto entry = m_selected_edges.find(id);
            m_selected_edges.erase(entry);
        }else {
            auto entry =  m_selected_cells.find(id);
            m_selected_cells.erase(entry);
        }

        // Delete Shape Element
        int shape_key = type * 114748364 + id;
        int shape_id = m_created_shapes[shape_key];

        Window::instance().rendering_mutex.unlock();
        Window::instance().remove_shape(shape_id);
        Window::instance().rendering_mutex.lock();
    }
    bool MeshObject::element_is_selected(int id, int type){

        id = type * 114748364 + id;

        auto it = m_selected_vertices.find(id);

        if(type == 0)
            return m_selected_faces.find(id) != m_selected_faces.end();
        else if(type == 1)
            return m_selected_vertices.find(id) != m_selected_vertices.end();
        else if(type == 2)
            return m_selected_edges.find(id) != m_selected_edges.end();
        else
            return m_selected_cells.find(id) != m_selected_cells.end();
    }

    void MeshObject::write_to_file(const std::string& file_path) const
    {
        OpenVolumeMesh::IO::FileManager file_manager;
        file_manager.writeFile(file_path, *m_mesh);
    }

    void MeshObject::set_mesh(OpenVolumeMesh::GeometryKernel <OpenVolumeMesh::Vec3f>* mesh)
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
            m_cylinder_vao = new VertexArrayObject(CommonMeshes::Cylinder::edge_cylinder().vertices(),
                                                   CommonMeshes::Cylinder::edge_cylinder().indices());
            // collect from-vertex and to-vertex for each edge
            std::vector<float> from_vertices;
            std::vector<float> to_vertices;
            m_edge_ids.clear();
            for (int i = 0; i < m_faces.size(); i += 3)
            {
                auto vh0 = OpenVolumeMesh::VertexHandle(m_faces[i + 0]);
                auto vh1 = OpenVolumeMesh::VertexHandle(m_faces[i + 1]);
                auto vh2 = OpenVolumeMesh::VertexHandle(m_faces[i + 2]);

                for (auto heh: m_mesh->outgoing_halfedges(vh0))
                {
                    auto out = m_mesh->to_vertex_handle(heh);
                    if (out == vh1)
                    {
                        m_edge_ids.push_back(m_mesh->edge_handle(heh).idx());
                        break;
                    }
                }
                for (auto heh: m_mesh->outgoing_halfedges(vh1))
                {
                    auto out = m_mesh->to_vertex_handle(heh);
                    if (out == vh2)
                    {
                        m_edge_ids.push_back(m_mesh->edge_handle(heh).idx());
                        break;
                    }
                }
                for (auto heh: m_mesh->outgoing_halfedges(vh2))
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

            for (auto halfface: m_mesh->face_halffaces(*f_it))
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
                    count++;
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
                    for (int i = 0; i < count; i++)
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
        if (m_face_ids.size() > value)
        {
            return m_face_ids[value] + 1;
        }
        return 0;
    }

    unsigned int MeshObject::to_edgeID(unsigned int value)
    {
        if (m_edge_ids.size() > value)
        {
            return m_edge_ids[value] + 1;
        }
        return 0;
    }

    void MeshObject::add_highlight(Highlight highlight)
    {
        // Remove Highlight if it already exists
        remove_highlight(highlight.v_h);

        // Add Highlight to Map
        highlight_map.insert({highlight.v_h,  highlight});
        /*
        //OpenVolumeMesh::VertexPropertyT<bool>  highlightProp = m_mesh->request_vertex_property<bool>("VertexHighlight");
        if (std::get<5>(tuple) == true)
        {
            // Add
            m_vertex_highlights.push_back(tuple);
        } else if (std::get<5>(tuple) == false)
        {
            // Remove
            auto pos = std::find(m_vertex_highlights.begin(), m_vertex_highlights.end(),
                                 tuple);
            if (pos != m_vertex_highlights.end())
            {
                m_vertex_highlights.erase(pos);
            }
        }
        */
    }


    void MeshObject::remove_highlight(OpenVolumeMesh::VertexHandle vh) {

        auto search = highlight_map.find(vh);
        if (search != highlight_map.end()) {
            // Element Exists
            highlight_map.erase(search);
        }else{
            // Element does not exist
            // ...
        }
    }

    void MeshObject::remove_highlights()
    {
        //highlight_map.clear();
    }

    std::map<OpenVolumeMesh::VertexHandle, Highlight>& MeshObject::get_highlights()
    {
        return highlight_map;
    }

    glm::vec3& MeshObject::get_mesh_offset()
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
            for (int i = 0; i < normals[*f_it].size(); i++)
            {
                m_face_normals.push_back(normals[*f_it][i]);
            }
        }

    }

    void MeshObject::init_vertex_normals()
    {
        m_vertex_normals.clear();

        std::vector<OpenVolumeMesh::HandleT<OpenVolumeMesh::Entity::Vertex>> m_face_vertices_array;

        OpenVolumeMesh::NormalAttrib normals(*m_mesh);
        normals.update_face_normals();

        // normal calculation based on OpenVolumeMesh's update_vertex_normals() method
        for (const auto& _vh: m_mesh->vertices())
        {
            std::set<std::pair<OpenVolumeMesh::HalfFaceHandle, float>> halffaces;
            for (auto voh_it = m_mesh->voh_iter(_vh); voh_it.valid(); ++voh_it)
            {
                for (auto hehf_it = m_mesh->hehf_iter(*voh_it); hehf_it.valid(); ++hehf_it)
                {
                    // find points of this halfface to calculate the angle of the face from the vertex
                    std::vector<glm::vec3> points;
                    for (auto hfv_it : m_mesh->halfface_vertices(*hehf_it))
                    {
                        if (hfv_it.idx() != _vh.idx())
                        {
                            auto point = m_mesh->vertex(hfv_it);
                            points.emplace_back(point[0], point[1], point[2]);
                        }
                    }

                    // calculate face angle
                    float angle = M_PI * 2.0f;
                    if (points.size() == 2)
                    {
                        auto p = m_mesh->vertex(_vh);
                        glm::vec3 pivot = glm::vec3(p[0], p[1], p[2]);
                        glm::vec3 first = glm::normalize(points[0] - pivot);
                        glm::vec3 second = glm::normalize(points[1] - pivot);

                        angle = glm::acos(glm::dot(first, second));
                    }
                    if (m_mesh->is_boundary(*hehf_it))
                    {
                        halffaces.insert(std::make_pair(*hehf_it, angle));
                    }
                }
            }

            // sum up normals of adjacent faces, but assign weight based on the angle size for better results
            auto normal = glm::vec3(0.0f);

            m_face_vertices_array.clear();

            for (auto halfface: halffaces)
            {
                glm::vec3 t = glm::vec3(0.0f);
                glm::vec3 b = glm::vec3(0.0f);
                auto n = m_mesh->normal(std::get<0>(halfface));
                float angle = std::get<1>(halfface);

                for(auto vert : m_mesh->halfface_vertices(std::get<0>(halfface)))
                {
                    m_face_vertices_array.push_back(vert);
                }

                normal += glm::vec3(n[0], n[1], n[2]) * (float) (angle / M_PI * 2.0f);
            }

            auto norm = glm::normalize(normal);
            m_vertex_normals.push_back(norm.x);
            m_vertex_normals.push_back(norm.y);
            m_vertex_normals.push_back(norm.z);
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

    int MeshObject::calculate_selection_size() const
    {
        // make sure that we choose the biggest possible vertex, edge or face id as the offset
        return (int) std::max(std::max(m_mesh->n_vertices(), m_mesh->n_edges()), m_mesh->n_faces());
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
        return (int) m_faces.size();
    }

    void MeshObject::set_selection_offset(int start)
    {
        m_selection_offset = {start, start + calculate_selection_size()};
    }
}

