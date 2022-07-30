#pragma once

#include "FrameBufferObject.h"

namespace volumeshOS::Internal
{

    class PrePassFrameBufferObject : public FrameBufferObject
    {

    public:

        PrePassFrameBufferObject(int width, int height): FrameBufferObject(width, height, {
            // depth
            FrameBufferAttachment{
                    .internal_format    = GL_DEPTH_COMPONENT,
                    .format             = GL_DEPTH_COMPONENT,
                    .type               = GL_FLOAT,
                    .attachment         = GL_DEPTH_ATTACHMENT,
                    .texture_filter     = GL_NEAREST,
                    .texture_wrap       = GL_CLAMP_TO_EDGE,
                    .texture_comp_func  = GL_LEQUAL,
                    .texture_comp_mode  = GL_NONE
            },
            // normal
            FrameBufferAttachment{
                    .internal_format    = GL_RGBA32F,
                    .format             = GL_RGBA,
                    .type               = GL_FLOAT,
                    .attachment         = GL_COLOR_ATTACHMENT0,
                    .texture_filter     = GL_NEAREST
            },
            // position
            FrameBufferAttachment{
                    .internal_format    = GL_RGBA32F,
                    .format             = GL_RGBA,
                    .type               = GL_FLOAT,
                    .attachment         = GL_COLOR_ATTACHMENT1,
                    .texture_filter     = -1,
                    .texture_wrap       = -1,
                    .texture_comp_func  = -1,
                    .texture_comp_mode  = -1,
                    .border_color       = -1,
                    .generate_mipmap    = true
            }
        }) {};

        [[nodiscard]] inline unsigned int get_depth_texture()
        {
            return get_texture(GL_DEPTH_ATTACHMENT);
        }

        [[nodiscard]] inline unsigned int get_normal_texture()
        {
            return get_texture(GL_COLOR_ATTACHMENT0);
        }

        [[nodiscard]] inline unsigned int get_position_texture()
        {
            return get_texture(GL_COLOR_ATTACHMENT1);
        }
    };
}
