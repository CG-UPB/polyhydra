#pragma once

#include "../Window.h"
#include "../rendering/Shader.h"
#include "../rendering/VertexArrayObject.h"
#include "../rendering/FrameBufferObject.h"

namespace vOS
{
    class PropertyView: public WindowPanel
    {
    public:
        PropertyView(const FrameBufferObject& fbo);
        ~PropertyView() override;
        void show() override;

    private:

        FrameBufferObject m_framebuffer;
        VertexArrayObject* m_vao;
        Shader* m_shader;
    };
}