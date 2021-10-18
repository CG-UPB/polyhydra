#include <OpenVolumeMesh/FileManager/FileManager.hh>
#include "mesh_object.h"


#include <array>
#include <vector>

namespace vOS
{
    MeshObject::MeshObject() {
        m_mesh = new OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f>();
    }

    void MeshObject::load_from_file(std::string file_path){
        OpenVolumeMesh::IO::FileManager file_manager;
        file_manager.readFile(file_path, *m_mesh);
    }

    void MeshObject::write_to_file(std::string file_path){
        OpenVolumeMesh::IO::FileManager file_manager;
        file_manager.writeFile(file_path, *m_mesh);
    }

    std::vector<float> MeshObject::vertices() {
        //int dim = m_mesh->dim();
        int dim = 3;
        std::vector<float> vertices (m_mesh->n_vertices() * dim);

        for(OpenVolumeMesh::VertexIter v_it = m_mesh->vertices_begin();
            v_it != m_mesh->vertices_end(); ++v_it) {

            auto myPoint = m_mesh->vertex(*v_it);
            for(int i = 0; i < dim; i++){
                vertices.push_back(myPoint[i]);
            }
        }

        return vertices;
    }

    std::vector<int> MeshObject::edges() {
        //int dim = m_mesh->dim();
        int dim = 3;
        std::vector<int> edges;

        for(OpenVolumeMesh::EdgeIter e_it = m_mesh->edges_begin();
            e_it != m_mesh->edges_end(); ++e_it){
            std::array<OpenVolumeMesh::VertexHandle, 2> vertexids = m_mesh->edge_vertices(*e_it);
            edges.push_back(vertexids[0].idx());
            edges.push_back(vertexids[1].idx());

        }

        return edges;
    }

/*
    std::vector<int> MeshObject::faces() {
        //int dim = m_mesh->dim();
        int dim = 3;
        std::vector<int> faces;

        for(OpenVolumeMesh::FaceIter f_it = m_mesh->faces_begin();
            f_it != m_mesh->vertices_end(); ++f_it) {

            ///TODO
        }

        return faces;
    }
*/
}

