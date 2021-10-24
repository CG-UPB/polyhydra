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
        m_vertexArrayObject = new VertexArrayObject(vertices(), edges());
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
        m_vertices.resize(m_mesh->n_vertices() * dim);

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
//        m_indices.clear();

//        for(OpenVolumeMesh::HalfEdgeIter he_it = m_mesh->halfedges_begin();
//            he_it != m_mesh->halfedges_end(); ++he_it){
//            std::array<OpenVolumeMesh::VertexHandle, 2> halfedge_vertexids = m_mesh->halfedge_vertices(*he_it);
//            m_indices.push_back(halfedge_vertexids[0].idx());
//            m_indices.push_back(halfedge_vertexids[1].idx());
//        }
//        bool stop = false;
//        m_indices.clear();
//        for(OpenVolumeMesh::VertexIter v_it = m_mesh->vertices_begin();
//            v_it != m_mesh->vertices_end(); ++v_it) {
//
//            for(OpenVolumeMesh::VertexOHalfEdgeIter voh_it = m_mesh->voh_iter(*v_it);
//                voh_it.valid(); ++voh_it) {
//
//                std::array<OpenVolumeMesh::VertexHandle, 2> halfedge_vertexids = m_mesh->halfedge_vertices(*voh_it);
//                int first = halfedge_vertexids[0].idx();
//                int second = halfedge_vertexids[1].idx();
//                m_indices.push_back(first);
//                m_indices.push_back(second);
//
//                int max = m_vertices.size() / 3 - 1;
//                if (first > max || second > max) {
//                    std::cout << "wrong index: first: " << first << ", second: " << second << std::endl;
//                }
//
//                if (m_indices.size() > m_vertices.size()) {
//                    stop = true;
//                    break;
//                }
//            }
//            if (stop) {
//                break;
//            }
//        }
//        std::cout << "vertices1: "<< m_vertices.size() <<std::endl;
//        std::cout << "indices1: "<< m_indices.size() <<std::endl;

        m_indices.clear();
        int index = 0;
        for (int i = 0; i < m_vertices.size(); i += 3) {
            for (int j = 0; j < std::rand() % 15; j++){
                m_indices.push_back(index);
                m_indices.push_back(std::rand() % (m_vertices.size() /3));
            }
            index++;
        }

        std::cout << "vertices2: "<< m_vertices.size() <<std::endl;
        std::cout << "indices2: "<< m_indices.size() <<std::endl;


        return m_indices;
    }


    std::vector<unsigned int> MeshObject::faces() {

        std::vector<unsigned int> faces;

        for(OpenVolumeMesh::FaceIter f_it = m_mesh->faces_begin();
            f_it != m_mesh->vertices_end(); ++f_it) {
            //<FaceVertexIter, FaceVertexIter> face_vertexids = m_mesh->face_vertices(*f_it);
            //faces.push_back(face_vertexids[0].idx());
            //faces.push_back(face_vertexids[1].idx());
        }

        return faces;
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

