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

        ~MeshObject() = default;
        static MeshObject *getInstance();

        OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> *m_mesh;

        void load_from_file(std::string file_path);
        void write_to_file(std::string file_path);

        void draw();

        std::vector<float> vertices();
        std::vector<unsigned int> edges();
        std::vector<unsigned int> faces();

        glm::vec3& getMeshOffset();

        void set_highlight(OpenVolumeMesh::VertexIter v_it, bool b);
        void set_highlight_color(OpenVolumeMesh::VertexIter v_it, OpenVolumeMesh::Vec3f col);
    private:
        MeshObject();
        static MeshObject *instance;

        void calculateMeshOffset();

        std::vector<float> m_vertices;
        std::vector<unsigned int> m_indices;
        VertexArrayObject* m_vertexArrayObject = nullptr;
        glm::vec3 m_meshOffsetFromCenter;
    };





}

#endif //VOLUMESHOS_MESH_OBJECT_H
