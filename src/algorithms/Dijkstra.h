#pragma once

#include <OpenVolumeMesh/Geometry/VectorT.hh>
#include "OpenVolumeMesh/Mesh/PolyhedralMesh.hh"

namespace vOS
{
    class Dijkstra
    {
    public:
        Dijkstra();
        void init();
        void init_vos();
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
        void reset_button_pressed();
        void debugging_template_ui();

        bool m_reset = false;
        bool m_pause = false;
        bool m_step = false;
        bool m_pause_toggled = false;
        bool m_open_file = false;
    };
}
