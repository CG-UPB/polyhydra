#pragma once

#include "../rendering/gl/PixelBufferObject.h"
#include "../ImguiRenderer.h"
#include "../rendering/passes/MeshPass.h"
#include "../rendering/passes/HighlightPass.h"
#include "../rendering/passes/BackgroundPass.h"
#include "../rendering/passes/SelectionPass.h"
#include "../rendering/passes/SelectionHoverPass.h"
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
        void m_take_screenshot(const std::string& filename);

    private:

        void handleResize();
        void handleMouseControl();
        void renderMesh(int mesh_id);
        void renderSelection();
        void querySelection(int type, int picked_id);

        [[nodiscard]] glm::vec3 get_arc_ball_vector(float x, float y) const;

        static const int SELECTION_TYPE_VERTEX = 1;
        static const int SELECTION_TYPE_EDGE = 2;
        static const int SELECTION_TYPE_FACE = 3;

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
        PixelBufferObject* m_pixel_buffer;
        FrameBufferObject* m_screen_quad_frameBuffer;
        RenderData m_render_data;

        bool m_zoom;
        glm::vec3 m_zoom_point;

        // render passes
        BackgroundPass m_background_pass;
        MeshPass m_mesh_pass;
        HighlightPass m_highlight_pass;
        ShapePass m_shape_pass;
        SelectionPass m_selection_pass;
        SelectionHoverPass m_selection_hover_pass;

        // render selection every 4th frame
        int m_frame_limit = 4;
        int m_current_frame = 0;
    };
}