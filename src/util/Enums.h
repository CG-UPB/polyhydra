#pragma once

namespace volumeshOS
{
    enum class CameraMode
    {
        ORBIT,
        FLY
    };

    enum class RenderingMode
    {
        WIREFRAME,
        ONLY_VERTICES,
        PHONG_FACE_NORMALS,
        PHONG_VERTEX_NORMALS,
    };

    enum class TransparencyMode
    {
        WEIGHTED_BLENDED,
        DEPTH_PEELING
    };

    enum class SSAOMode
    {
        OFF,
        QUALITY,
        BALANCED,
        PERFORMANCE,
        CUSTOM
    };

    enum class SelectionMode
    {
        OFF,
        VERTEX,
        EDGE,
        FACE,
        CELL,
        ALL
    };

    enum class LightingMode
    {
        PHONG,
        PBR
    };

}