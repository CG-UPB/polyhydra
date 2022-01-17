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
#include "../settings/GlobalViewerSettings.h"

namespace vOS
{

    MeshObject::MeshObject()
    {
        m_mesh = new OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f>();

        BufferSpecification spec;
        spec.peel_depth = 0;
        spec.slice_depth = 0;

        m_mvb = new MeshVertexBuffer(m_mesh, spec);

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
        bool already_selected = is_element_selected(id, type);
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
        bool is_selected = is_element_selected(id, type);
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
    bool MeshObject::is_element_selected(int id, int type){

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

        OpenVolumeMesh::CellPropertyT<int> peel_property = m_mesh->request_cell_property<int>("PeelDepth");
        peel_property->set_persistent(true);
        calculate_peel_depth();

        remove_highlights();
        m_should_update = true;
        calculate_mesh_offset();
        GlobalViewerSettings::getInstance()->m_new_Mesh();
    }

    void MeshObject::update_vertex_buffer()
    {
        int current_peel_level = GlobalViewerSettings::getInstance()->m_get_current_mesh_peel_level();
        int current_slice_level = GlobalViewerSettings::getInstance()->m_get_current_mesh_slice_level();
        if (m_should_update)
        {
            BufferSpecification spec;
            spec.peel_depth = current_peel_level;
            m_mvb = new MeshVertexBuffer(m_mesh, spec);
        }
        m_should_update = false;
    }

    void MeshObject::calculate_mesh_offset()
    {
        std::vector<float> vertices;

        for (auto v_it : m_mesh->vertices())
        {
            auto v_pos = m_mesh->vertex(v_it);
            vertices.push_back(v_pos[0]);
            vertices.push_back(v_pos[1]);
            vertices.push_back(v_pos[2]);
        }

        glm::vec3 min(vertices[0], vertices[1], vertices[2]);
        glm::vec3 max(vertices[0], vertices[1], vertices[2]);
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
        m_min = min;
        m_max = max;
        m_mesh_offset_from_center = min + (max - min) * 0.5f;
    }

    void MeshObject::calculate_peel_depth() const
    {
        OpenVolumeMesh::CellPropertyT<int> cell_peel_property = m_mesh->request_cell_property<int>("PeelDepth");
        OpenVolumeMesh::VertexPropertyT<int> vertex_peel_property = m_mesh->request_vertex_property<int>("PeelDepth");

        std::vector<OpenVolumeMesh::VertexHandle> act_level;
        std::vector<OpenVolumeMesh::VertexHandle> next_level;

        for(auto vertex : m_mesh->vertices())
        {
            if(m_mesh->is_boundary(vertex))
            {
                vertex_peel_property[vertex] = 0;
                act_level.push_back(vertex);
            }
            else
            {
                vertex_peel_property[vertex] = -1;
            }
        }

        int depth = 0;

        while(!act_level.empty())
        {
            depth++;
            for(auto vertex : act_level)
            {
                for (auto neighbour_cell : m_mesh->vertex_cells(vertex))
                {
                    for(auto neighbour : m_mesh->cell_vertices(neighbour_cell))
                    {
                        if (vertex_peel_property[neighbour] == -1)
                        {
                            vertex_peel_property[neighbour] = depth;
                            next_level.push_back(neighbour);
                        }
                    }
                }
            }
            act_level.clear();
            act_level.insert(act_level.begin(), next_level.begin(), next_level.end());
            next_level.clear();
        }

        for(auto cell : m_mesh->cells())
        {
            int minimum = 100000;
            for(auto cell_vertex : m_mesh->cell_vertices(cell))
            {
                if(vertex_peel_property[cell_vertex] < minimum)
                {
                    minimum = vertex_peel_property[cell_vertex];
                }
            }
            cell_peel_property[cell] = minimum;
        }
    }

    int MeshObject::to_vertexID(int value)
    {
        return m_mvb->to_vertexID(value);
    }

    int MeshObject::to_edgeID(int value)
    {
        return m_mvb->to_edgeID(value);
    }

    int MeshObject::to_faceID(int value)
    {
        return m_mvb->to_faceID(value);
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

    VertexArrayObject* MeshObject::get_vao() const
    {
        return m_mvb->get_vao();
    }

    VertexArrayObject* MeshObject::get_sphere_vao() const
    {
        return m_mvb->get_sphere_vao();
    }

    VertexArrayObject* MeshObject::get_cylinder_vao() const
    {
        return m_mvb->get_cylinder_vao();
    }

    int MeshObject::calculate_selection_size() const
    {
        // make sure that we choose the biggest possible vertex, edge or face id as the offset
        return (int) m_mesh->n_cells() * 24;
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

    std::pair<glm::vec3,glm::vec3>& MeshObject::get_transformed_bb(const glm::mat4& transform)
    {
        if (m_data.m_slice_locked)
        {
            return m_transformed_bb;
        }

        std::vector<float> vertices;

        for (auto v_it : m_mesh->vertices())
        {
            auto v_pos = m_mesh->vertex(v_it);
            glm::vec4 vec(v_pos[0], v_pos[1], v_pos[2], 1.0);
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
        glm::vec3 m1(glm::inverse(transform) * min);
        glm::vec3 m2(glm::inverse(transform) * max);
        m_transformed_bb = std::make_pair(m1,m2);
        return m_transformed_bb;
    }


    glm::vec3& MeshObject::get_slice_dir(const glm::mat4& transform, const glm::vec3& view_dir)
    {
        if (!m_data.m_slice_locked)
        {
            m_just_locked = true;
            m_slice_dir = view_dir;
        }
        else
        {
            if (m_just_locked)
            {
                m_just_locked = false;
                m_slice_dir = glm::vec3{glm::inverse(transform) * glm::vec4(view_dir, 0.0)};
            }
        }
        return m_slice_dir;
    }

    MeshObject::~MeshObject()
    {
        for (auto mvb : m_peel_cache)
        {
            delete mvb.second;
        }
    }
}

