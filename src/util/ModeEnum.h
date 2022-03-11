#pragma once

namespace vOS
{
    enum ModeEnum {
        Wireframe = 0,
        Only_Vertices = 1,
        Phong_Facenormals = 2,
        Phong_Vertexnormals = 3,
    };

    enum Transparency {
        WEIGHTED_BLENDED = 0,
        DEPTH_PEELING = 1
    };

    enum SSAO_Mode {
        OFF = 0,
        QUALITY = 1,
        BALANCED = 2,
        PERFORMANCE = 3,
        CUSTOM = 4
    };
}