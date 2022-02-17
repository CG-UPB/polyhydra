#pragma once

#include "../rendering/gl/PixelBufferObject.h"
#include "../rendering/gl/PrePassFrameBufferObject.h"
#include "../ImguiRenderer.h"
#include "../rendering/passes/MeshPass.h"
#include "../rendering/passes/SSAOPass.h"
#include "../rendering/passes/BackgroundPass.h"
#include "../rendering/passes/SelectionPass.h"
#include "../rendering/passes/SelectionHoverPass.h"
#include "../rendering/passes/PrePass.h"
#include "../rendering/passes/TransparencyPass_WB.h"
#include "../rendering/passes/TransparencyPass_DP.h"
#include "../Window.h"
#include "../rendering/passes/ShapePass.h"

namespace vOS
{
    class TransparencyPass_WB;
    class TransparencyPass_DP;
    class MeshPass;
    class SSAOPass;

    class MeshView: public WindowPanel
    {
    public:

        MeshView(int width, int height);
        ~MeshView() override;
        void show() override;
        void m_take_screenshot(const std::string& filename);
        void set_zoom_point(glm::vec3 zoom_point);
    private:

        void handleResize();
        void handleMouseControl();
        void renderMesh(int mesh_id);
        void renderSelection();
        void querySelection(int type, int picked_id);
        void render_pre_pass();
        void render_ssao_pass();
        void render_debug_menu();
        void render_transparency_wb();
        void render_transparency_dp();
        [[nodiscard]] unsigned int get_selected_texture();

        [[nodiscard]] glm::vec3 get_arc_ball_vector(float x, float y) const;

        // these are just to differentiate between the different textures, we cannot directly use the texture ids
        // since they can change when resizing the framebuffer
        static const int FINAL_IMAGE   = 0;
        static const int SELECTION     = 1;
        static const int SSAO_PRE      = 2;
        static const int SSAO_BLUR     = 3;

        static const int SELECTION_TYPE_VERTEX = 1;
        static const int SELECTION_TYPE_EDGE = 2;
        static const int SELECTION_TYPE_FACE = 3;
        int m_hovered_element_id = 0;
        int m_hovered_element_type = 0;

        // used for the arc ball
        bool m_arcBallOn;
        bool m_lastDown;
        double m_lastX;
        double m_lastY;

        int m_viewportPanelWidth;
        int m_viewportPanelHeight;

        // opengl rendering
        FrameBufferObject* m_meshFrameBuffer = nullptr;
        FrameBufferObject* m_selectionFrameBuffer = nullptr;
        FrameBufferObject* m_screen_quad_frameBuffer = nullptr;
        PixelBufferObject* m_pixel_buffer = nullptr;
        RenderData m_render_data;

        // selected texture for rendering (mostly for debugging)
        int m_viewport_texture = FINAL_IMAGE;

        // camera variables
        glm::vec3 m_previous_movement_vector;
        float m_movement_speed_multiplier = 1;
        bool m_zoom;
        glm::vec3 m_zoom_point;

        // render passes
        BackgroundPass m_background_pass;
        PrePass* m_pre_pass = nullptr;
        MeshPass* m_mesh_pass = nullptr;
        SSAOPass* m_ssao_pass = nullptr;
        ShapePass m_shape_pass;
        SelectionPass m_selection_pass;
        SelectionHoverPass m_selection_hover_pass;
        TransparencyPass_WB* m_transparency_pass_wb = nullptr;
        TransparencyPass_DP* m_transparency_pass_dp = nullptr;

        // render selection every 4th frame
        int m_frame_limit = 4;
        int m_current_frame = 0;

        friend class MeshPass;
        friend class SSAOPass;
        friend class TransparencyPass_WB;
        friend class TransparencyPass_DP;
    };
}