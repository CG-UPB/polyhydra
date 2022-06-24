
#pragma once

#include "RenderPass.h"
#include "../gl/PrePassFrameBufferObject.h"

namespace volumeshOS
{

    class PrePass : public RenderPass
    {

    public:

        PrePass(int width, int height);

        void resize_buffers(int width, int height);
        void clear_position_buffer(const RenderData& data);

        void render(std::shared_ptr<VertexArrayObject> vao, const RenderData& data, std::shared_ptr<MeshObject> mesh) override;

        [[nodiscard]] std::shared_ptr<PrePassFrameBufferObject> get_framebuffer() const;

    private:

        std::shared_ptr<Shader> m_clear_position_shader;
        std::shared_ptr<PrePassFrameBufferObject> m_pre_pass_framebuffer;
    };
}
