
#pragma once

#include "RenderPass.h"

namespace vOS
{

    class PrePass : public RenderPass
    {

    public:

        PrePass(int width, int height);
        ~PrePass();

        void resize_buffers(int width, int height);

        void render(VertexArrayObject* vao, const RenderData& data, int mesh_id) override;

        [[nodiscard]] PrePassFrameBufferObject* get_framebuffer() const;

    private:

        PrePassFrameBufferObject* m_pre_pass_framebuffer = nullptr;
    };
}
