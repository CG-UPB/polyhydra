#pragma once

#include "../Window.h"
#include "../rendering/passes/MeshPass.h"
#include "../rendering/passes/HighlightPass.h"

namespace vOS
{
    class MeshView: public WindowPanel
    {
    public:
        MeshView(int width, int height);
        ~MeshView() override;
        void show() override;

    private:

        void handleResize();
        void handleMouseControl();
        void renderMesh();

        // used for the arc ball
        bool m_arcBallOn;
        bool m_lastDown;
        bool m_W_button_pressed;
        double m_lastX;
        double m_lastY;

        int m_viewportPanelWidth;
        int m_viewportPanelHeight;

        // opengl rendering
        FrameBufferObject* m_meshFrameBuffer;
        RenderData m_render_data;
        MeshPass m_mesh_pass;
        HighlightPass m_highlight_pass;
    };
}