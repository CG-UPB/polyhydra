#include <OpenVolumeMesh/Geometry/VectorT.hh>
#include <OpenVolumeMesh/Core/GeometryKernel.hh>

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

        std::vector<float> vertices();
        std::vector<unsigned int> edges();
        std::vector<unsigned int> faces();

        void set_highlight(OpenVolumeMesh::VertexIter v_it, bool b);
        void set_highlight_color(OpenVolumeMesh::VertexIter v_it, OpenVolumeMesh::Vec3f col);
    private:
        MeshObject();
        static MeshObject *instance;


    };





}

#endif //VOLUMESHOS_MESH_OBJECT_H
