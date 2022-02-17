#pragma once

#include "FrameBufferObject.h"

namespace vOS {

    class PrePassFrameBufferObject : public FrameBufferObject {

    public:

        PrePassFrameBufferObject(int width, int height);

        [[nodiscard]] unsigned int get_depth_texture();
        [[nodiscard]] unsigned int get_normal_texture();
        [[nodiscard]] unsigned int get_position_texture();
    };
}
