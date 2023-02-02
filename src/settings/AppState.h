#pragma once

#include "../panels/ToolBar.h"
#include "../mesh/MeshObject.h"
#include "../util/Enums.h"
#include "volumeshOS.h"

namespace volumeshOS::Internal
{
    struct CameraOptions
    {
        CameraMode mode         = CameraMode::ORBIT;
        glm::vec3 position      = {0.0f, 0.0f, 13.0f};
        float fov               = 45.0f;
    };

    struct LightOptions
    {
        glm::vec3 direction     = {0.5f, 1.0f, 1.0f};
        glm::vec3 color         = {0.94f, 0.86f, 0.75f};
        float intensity         = 5.0f;
    };

    struct ShadowOptions
    {
        float shadow_strength    = 0.8;
        float penumbra_scale     = 1.0f;
        float softness           = 1.0f;
    };

    struct SkyOptions
    {
        glm::vec3 sky_color     = {0.5f, 0.75f, 0.8f};
        float fog_density       = 0.02f;
        glm::vec3 fog_color     = {0.5f, 0.6f, 0.7f};
    };

    struct ShapeOptions
    {
        float ambient_strength  = 1.0f;
        float diffuse_strength  = 1.0f;
        float specular_strength = 0.15f;
        float specular_exponent = 8.0f;
        bool use_pbr            = true;
        float metallic          = 0.15;
        float roughness         = 0.65;
    };

    struct GroundOptions
    {
        bool visible            = true;
        bool solid              = true;
        bool grid               = true;
        glm::vec3 solid_color   = {1.0, 1.0, 1.0};
        glm::vec3 grid_color    = {0.67, 0.67, 0.67};
        float height            = -5.0f;
        int size                = 1000;
        int tiles               = 200;
        bool use_pbr            = true;
        float metallic          = 0.1f;
        float roughness         = 0.65f;
    };

    struct SSAOOptions
    {
        bool active             = true;
        int num_samples         = 64;
        float sample_radius     = 1.5f;
        float strength          = 1.5f;
        float z_bias            = 0.01f;
        float blur_sharpness    = 1.0f;
    };

    struct PostProcessingOptions
    {
        bool active         = true;
        float gamma         = 2.2f;
        float saturation    = 1.0f;
        float contrast      = 1.0f;
    };

    struct OutlineOptions
    {
        float width         = 5.0f;
        glm::vec4 color     = {1.0f, 0.3f, 0.0f, 1.0f};
    };

    struct AppState
    {
        static struct AppSettings
        {
            RenderingMode rendering_mode            = RenderingMode::PHONG_FACE_NORMALS;
            TransparencyMode transparency_mode      = TransparencyMode::DEPTH_PEELING;
            SelectionMode selection_mode            = SelectionMode::OFF;
            SSAOMode ssao_mode                      = SSAOMode::QUALITY;
            SSAOOptions ssao_custom                 = {};
            ShapeOptions shapes                     = {};
            GroundOptions ground                    = {};
            CameraOptions camera                    = {};
            ShadowOptions shadow                    = {};
            LightOptions light                      = {};
            SkyOptions sky                          = {};
            PostProcessingOptions post_processing   = {};
            OutlineOptions outline                  = {};
            bool shapes_active                      = true;
            bool selection_active                   = false;
            bool transparency_active                = false;
            bool ssao_active                        = false;
            bool shadows_active                     = false;
            bool isolation_active                   = false;
            bool digging_active                     = false;
            int num_depth_peeling_passes            = 15;
            int num_shadow_cascades                 = 8;
            float wireframe_size                    = 1.2f;
            float vertex_size                       = 0.4f;
            bool use_global_pbr                     = true;
            bool block_input                        = false;
            bool multisampling                      = true;
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

        static struct RenderStatistics
        {
            int draw_calls_per_frame                = 0;
            int total_rendered_vertices_per_frame   = 0;
            int total_rendered_triangles_per_frame  = 0;
        } statistics;
    };

}
