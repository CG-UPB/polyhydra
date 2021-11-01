#include <OpenVolumeMesh/Geometry/VectorT.hh>
#include <OpenVolumeMesh/Core/GeometryKernel.hh>
#include <vector>
#include "../rendering/VertexArrayObject.h"
#include "glm/gtx/transform.hpp"

#ifndef VOLUMESHOS_MESH_OBJECT_H
#define VOLUMESHOS_MESH_OBJECT_H

namespace vOS
{
    class MeshObject
    {
    public:

        MeshObject();
        ~MeshObject() = default;

        bool m_should_update;
        OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> *m_mesh;

        void load_from_file(std::string file_path);
        void write_to_file(std::string file_path);
        void set_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> *mesh);

        void draw();

        std::vector<float> vertices();
        std::vector<unsigned int> edges();
        std::vector<unsigned int> faces();

        glm::vec3& get_mesh_offset();

        void set_highlight(OpenVolumeMesh::VertexIter v_it, bool b);
        void set_highlight_color(OpenVolumeMesh::VertexIter v_it, OpenVolumeMesh::Vec3f col);
    private:

        void calculate_mesh_offset();
        void update_vertex_buffer();

        std::vector<float> m_vertices;
        std::vector<unsigned int> m_indices;
        VertexArrayObject* m_vertexArrayObject = nullptr;
        glm::vec3 m_mesh_offset_from_center;
    };





}

#endif //VOLUMESHOS_MESH_OBJECT_H
