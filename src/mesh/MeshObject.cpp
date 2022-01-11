#include "MeshObject.h"

#include <OpenVolumeMesh/Attribs/OpenVolumeMeshStatus.hh>
#include <OpenVolumeMesh/Attribs/NormalAttrib.hh>
#include <OpenVolumeMesh/Attribs/ColorAttrib.hh>
#include <OpenVolumeMesh/FileManager/FileManager.hh>
#include "../panels/LogWindow.h"
#include <array>
#include <string>
#include <unordered_set>
#include "../Window.h"
#include "../rendering/meshes/CommonMeshes.h"
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
        update_vertex_buffer();
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
            calculate_mesh_offset();
        }
        m_should_update = false;
    }

    void MeshObject::calculate_mesh_offset()
    {
        auto vertices = m_mvb->get_original_vertices();

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

    void MeshObject::calculate_peel_depth()
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
                for (auto neighbour : m_mesh->vertex_vertices(vertex))
                {
                    if(vertex_peel_property[neighbour] == -1)
                    {
                        vertex_peel_property[neighbour] = depth;
                        next_level.push_back(neighbour);
                    }
                }
            }
            act_level.clear();
            for(auto vertex : next_level)
            {
                act_level.push_back(vertex);
            }
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
        return (int) m_mvb->get_num_selection_vertices() * 3;
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

    MeshObject::~MeshObject()
    {
        for (auto mvb : m_peel_cache)
        {
            delete mvb.second;
        }
    }
}

