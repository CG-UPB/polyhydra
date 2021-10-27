#include <OpenVolumeMesh/FileManager/FileManager.hh>
#include "mesh_object.h"


#include <array>
#include <random>

#include <OpenVolumeMesh/Attribs/OpenVolumeMeshStatus.hh>

namespace vOS
{
    MeshObject *MeshObject::instance = 0;

    MeshObject *MeshObject::getInstance() {
        if (instance == 0)
        {
            instance = new MeshObject();
        }

        return instance;

    }


    MeshObject::MeshObject() {
        m_mesh = new OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f>();

        OpenVolumeMesh::VertexPropertyT<bool> highlightProp = m_mesh->request_vertex_property<bool>("VertexHighlight");
        highlightProp->set_persistent(true);

        OpenVolumeMesh::VertexPropertyT<OpenVolumeMesh::Vec3f> highlightColProp = m_mesh->request_vertex_property<OpenVolumeMesh::Vec3f>("VertexHighlightColor");
        highlightColProp->set_persistent(true);

    }

    void MeshObject::load_from_file(std::string file_path){
        OpenVolumeMesh::IO::FileManager file_manager;
        file_manager.readFile(file_path, *m_mesh);

        if (m_vertexArrayObject != nullptr) {
            delete m_vertexArrayObject;
        }
        m_vertexArrayObject = new VertexArrayObject(vertices(), faces());
    }

    void MeshObject::write_to_file(std::string file_path){
        OpenVolumeMesh::IO::FileManager file_manager;
        file_manager.writeFile(file_path, *m_mesh);
    }

    std::vector<float> MeshObject::vertices() {
        //int dim = m_mesh->dim();
        int dim = 3;
        //std::vector<float> vertices (m_mesh->n_vertices() * dim);

        m_vertices.clear();
        m_vertices.reserve(m_mesh->n_vertices() * dim);

        for(OpenVolumeMesh::VertexIter v_it = m_mesh->vertices_begin();
            v_it != m_mesh->vertices_end(); ++v_it) {

            auto myPoint = m_mesh->vertex(*v_it);
            for(int i = 0; i < dim; i++){
                m_vertices.push_back(myPoint[i]);
            }
        }

        return m_vertices;
    }

    std::vector<unsigned int> MeshObject::edges() {
        m_indices.clear();

        for(OpenVolumeMesh::EdgeIter e_it = m_mesh->edges_begin();
            e_it != m_mesh->edges_end(); ++e_it){
            std::array<OpenVolumeMesh::VertexHandle, 2> edge_vertexids = m_mesh->edge_vertices(*e_it);
            m_indices.push_back(edge_vertexids[0].idx());
            m_indices.push_back(edge_vertexids[1].idx());
        }

        return m_indices;
    }


    std::vector<unsigned int> MeshObject::faces() {

        m_indices.clear();

        for(OpenVolumeMesh::FaceIter f_it = m_mesh->faces_begin();
            f_it != m_mesh->vertices_end(); ++f_it) {
            std::pair<OpenVolumeMesh::FaceVertexIter, OpenVolumeMesh::FaceVertexIter> face_vertexids = m_mesh->face_vertices(*f_it);
            for(OpenVolumeMesh::FaceVertexIter fv_it = face_vertexids.first;
            fv_it != face_vertexids.second; ++fv_it){
                m_indices.push_back(fv_it->idx());
            }
        }

        return m_indices;
    }

    void MeshObject::draw() {
        if (m_vertexArrayObject != nullptr) {
            m_vertexArrayObject->draw();
        }
    }


    void MeshObject::set_highlight(OpenVolumeMesh::VertexIter v_it, bool b){
        OpenVolumeMesh::VertexPropertyT<bool>  highlightProp = m_mesh->request_vertex_property<bool>("VertexHighlight");
        highlightProp[*v_it] = b;
    }

    void MeshObject::set_highlight_color(OpenVolumeMesh::VertexIter v_it, OpenVolumeMesh::Vec3f col){
        OpenVolumeMesh::VertexPropertyT<OpenVolumeMesh::Vec3f> highlightColProp = m_mesh->request_vertex_property<OpenVolumeMesh::Vec3f>("VertexHighlightColor");
        highlightColProp[*v_it] = col;
    }


}

