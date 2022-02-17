
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
        void clear_position_buffer(const RenderData& data);

        void render(VertexArrayObject* vao, const RenderData& data, int mesh_id) override;

        [[nodiscard]] PrePassFrameBufferObject* get_framebuffer() const;

    private:

        Shader* m_clear_position_shader = nullptr;
        PrePassFrameBufferObject* m_pre_pass_framebuffer = nullptr;
    };
}
