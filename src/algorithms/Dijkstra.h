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
        void start();
        void linear_run();
        void parallel_run();
        void step();

    private:

        OpenVolumeMesh::VertexHandle m_start;
        OpenVolumeMesh::VertexHandle m_end;
        OpenVolumeMesh::GeometricPolyhedralMeshV3f m_mesh;
        OpenVolumeMesh::EdgePropertyT<float> m_weights;

        void pause_button_released();
        void pause_button_pressed();
        void step_button_pressed();
        void reset_button_pressed();
        void debugging_template_ui_linear();
        void debugging_template_ui_parallel();

        bool m_reset = false;
        bool m_pause = false;
        bool m_step = false;
        bool m_pause_toggled = false;
        bool m_open_file = false;

    };
}
