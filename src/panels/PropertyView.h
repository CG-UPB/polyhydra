#pragma once

#include "../ImguiRenderer.h"
#include "../rendering/Shader.h"
#include "../rendering/VertexArrayObject.h"
#include "../rendering/FrameBufferObject.h"
#include "MeshView.h"

namespace vOS
{
    class PropertyView: public WindowPanel
    {
    public:
        explicit PropertyView(const MeshView& mesh_view);
        ~PropertyView() override;
        void show() override;

    private:

        const MeshView& m_mesh_view;

        VertexArrayObject* m_vao;
        Shader* m_shader;
    };
}