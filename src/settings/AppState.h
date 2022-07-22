#pragma once

#include "../panels/ToolBar.h"
#include "../mesh/MeshObject.h"
#include "../util/Enums.h"
#include "volumeshOS.h"

namespace volumeshOS::Internal
{
    struct SSAOOptions
    {
        bool active         = true;
        int num_samples     = 64;
        float sample_radius = 0.5f;
        float strength      = 1.5f;
        float z_bias        = 0.01f;
    };

    struct AppState
    {
        static struct AppSettings
        {
            RenderingMode rendering_mode        = RenderingMode::PHONG_FACE_NORMALS;
            TransparencyMode transparency_mode  = TransparencyMode::WEIGHTED_BLENDED;
            SelectionMode selection_mode        = SelectionMode::OFF;
            SSAOMode ssao_mode                  = SSAOMode::QUALITY;
            SSAOOptions ssao_custom_options     = {};
            bool selection_active               = false;
            bool transparency_active            = false;
            bool ssao_active                    = true;
            bool shadows_active                 = false;
            bool isolation_active               = false;
            bool digging_active                 = false;
            int num_depth_peeling_passes        = 12;
            int num_shadow_cascades             = 3;
            float wireframe_size                = 0.35f;
            float vertex_size                   = 0.4f;
        } settings;

        static void restore_default_settings()
        {
            settings = AppSettings{};
        }

        static struct AppCallbacks
        {
            std::function<void()> on_gui_render                                                 = [](){};
            std::function<void(const VMesh, OpenVolumeMesh::CellHandle)> on_cell_hover          = [](const VMesh, OpenVolumeMesh::CellHandle){};
            std::function<void(const VMesh, OpenVolumeMesh::FaceHandle)> on_face_hover          = [](const VMesh, OpenVolumeMesh::FaceHandle){};
            std::function<void(const VMesh, OpenVolumeMesh::HalfFaceHandle)> on_halfface_hover  = [](const VMesh, OpenVolumeMesh::HalfFaceHandle){};
            std::function<void(const VMesh, OpenVolumeMesh::EdgeHandle)> on_edge_hover          = [](const VMesh, OpenVolumeMesh::EdgeHandle){};
            std::function<void(const VMesh, OpenVolumeMesh::VertexHandle)> on_vertex_hover      = [](const VMesh, OpenVolumeMesh::VertexHandle){};
            std::function<void(const VMesh, OpenVolumeMesh::CellHandle)> on_cell_select         = [](const VMesh, OpenVolumeMesh::CellHandle){};
            std::function<void(const VMesh, OpenVolumeMesh::FaceHandle)> on_face_select         = [](const VMesh, OpenVolumeMesh::FaceHandle){};
            std::function<void(const VMesh, OpenVolumeMesh::HalfFaceHandle)> on_halfface_select = [](const VMesh, OpenVolumeMesh::HalfFaceHandle){};
            std::function<void(const VMesh, OpenVolumeMesh::EdgeHandle)> on_edge_select         = [](const VMesh, OpenVolumeMesh::EdgeHandle){};
            std::function<void(const VMesh, OpenVolumeMesh::VertexHandle)> on_vertex_select     = [](const VMesh, OpenVolumeMesh::VertexHandle){};
            std::function<void(float, float, float)> on_position_select                         = [](float, float, float){};
        } callbacks;

        static void restore_default_callbacks()
        {
            callbacks = AppCallbacks{};
        }
    };

}
