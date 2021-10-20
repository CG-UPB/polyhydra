//
// Created by jan on 15.10.21.
//

#ifndef VOLUMESHOS_DIJKSTRA_H
#define VOLUMESHOS_DIJKSTRA_H

#include <OpenVolumeMesh/Geometry/VectorT.hh>
#include "OpenVolumeMesh/Mesh/PolyhedralMesh.hh"

namespace vOS
{
    class Dijkstra
    {
    public:
        void init();
        void run();
        void step();

    private:
        OpenVolumeMesh::VertexHandle m_start;
        OpenVolumeMesh::VertexHandle m_end;
        OpenVolumeMesh::GeometricPolyhedralMeshV3f m_mesh;
    };
}


#endif //VOLUMESHOS_DIJKSTRA_H
