#pragma once

#include <OpenVolumeMesh/Geometry/VectorT.hh>
#include "OpenVolumeMesh/Mesh/PolyhedralMesh.hh"

namespace vOS
{
    class Dijkstra
    {
    public:
        Dijkstra();
        void init(OpenVolumeMesh::VertexHandle start, OpenVolumeMesh::VertexHandle end);
        void run();
        void step();

    private:

        OpenVolumeMesh::VertexHandle m_start;
        OpenVolumeMesh::VertexHandle m_end;
        OpenVolumeMesh::GeometricPolyhedralMeshV3f m_mesh;
        OpenVolumeMesh::EdgePropertyT<float> m_weights;

        void PauseButtonReleased();
        void PauseButtonPressed();
        void step_button_pressed();

        bool m_pause = true;
        bool m_step = false;
    };
}
