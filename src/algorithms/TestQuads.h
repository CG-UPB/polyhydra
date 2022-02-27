//
// Created by lukas on 24.02.22.
//

#ifndef VOLUMESHOS_TESTQUADS_H
#define VOLUMESHOS_TESTQUADS_H

#include <OpenVolumeMesh/Geometry/VectorT.hh>
#include "OpenVolumeMesh/Mesh/PolyhedralMesh.hh"

class TestQuads
{
public:
    TestQuads();
    void run();
    void ui();
    void add_quad();
    OpenVolumeMesh::GeometricPolyhedralMeshV3d create_quad(std::vector<OpenVolumeMesh::Vec3d> positions);


private:
    std::vector<OpenVolumeMesh::Vec3d> m_quad_positions;
    std::vector<OpenVolumeMesh::Vec3d> m_ground_positions;

};


#endif //VOLUMESHOS_TESTQUADS_H
