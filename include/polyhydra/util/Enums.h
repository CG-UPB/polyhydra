#pragma once

namespace polyhydra
{
enum class CameraMode
{
    ORBIT,
    FLY
};

enum class RenderingMode
{
    POINTS,
    LINES,
    CELLS,
};

enum class ShadingMode
{
    FLAT,
    PHONG,
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

} // namespace polyhydra