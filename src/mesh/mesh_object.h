#include <OpenVolumeMesh/Geometry/VectorT.hh>
#include <OpenVolumeMesh/Core/GeometryKernel.hh>

#ifndef VOLUMESHOS_MESH_OBJECT_H
#define VOLUMESHOS_MESH_OBJECT_H

namespace vOS
{

    class MeshObject
    {
    public:
        MeshObject();
        ~MeshObject() = default;

        OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> *m_mesh;

        void load_from_file(std::string file_path);
        void write_to_file(std::string file_path);

        std::vector<float> vertices();
        std::vector<int> edges();
        //std::vector<int> faces();


    };


}

#endif //VOLUMESHOS_MESH_OBJECT_H
