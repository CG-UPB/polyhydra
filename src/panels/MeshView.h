#pragma once

#include "../ImguiRenderer.h"
#include "../rendering/passes/MeshPass.h"
#include "../rendering/passes/HighlightPass.h"
#include "../rendering/passes/BackgroundPass.h"
#include "../rendering/passes/SelectionPasss.h"
#include "../Window.h"
#include "../rendering/passes/ShapePass.h"

namespace vOS
{
    class MeshView: public WindowPanel
    {
    public:
        MeshView(int width, int height);
        ~MeshView() override;
        void show() override;
        void set_mesh_object(MeshObject* mesh_pointer) {m_mesh = mesh_pointer;};
    private:

        void handleResize();
        void handleMouseControl();
        void renderMesh();
        void renderSelection();

        glm::vec3 get_arc_ball_vector(float x, float y) const;
        MeshObject* m_mesh;

        // used for the arc ball
        bool m_arcBallOn;
        bool m_lastDown;
        double m_lastX;
        double m_lastY;

        int m_viewportPanelWidth;
        int m_viewportPanelHeight;

        // opengl rendering
        FrameBufferObject* m_meshFrameBuffer;
        FrameBufferObject* m_selectionFrameBuffer;
        RenderData m_render_data;

        // render passes
        BackgroundPass m_background_pass;
        MeshPass m_mesh_pass;
        HighlightPass m_highlight_pass;
        ShapePass m_shape_pass;
        SelectionPass m_selection_pass;
    };
}