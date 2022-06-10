#pragma once

#include "vospch.h"

#include "../rendering/gl/PixelBufferObject.h"
#include "../rendering/gl/PrePassFrameBufferObject.h"
#include "../ImguiRenderer.h"
#include "../rendering/passes/MeshPass.h"
#include "../rendering/passes/SSAOPass.h"
#include "../rendering/passes/BackgroundPass.h"
#include "../rendering/passes/SelectionPass.h"
#include "../rendering/passes/SelectionHoverPass.h"
#include "../rendering/passes/PrePass.h"
#include "../rendering/passes/ShadowMapPass.h"
#include "../rendering/passes/ShadowColorFilterPass.h"
#include "../rendering/passes/TransparentShadowMapPass.h"
#include "../rendering/passes/TransparencyPass_WB.h"
#include "../rendering/passes/TransparencyPass_DP.h"
#include "../rendering/passes/VertexOnlyPass.h"
#include "../rendering/passes/ShapePass.h"
#include "rendering/camera/MeshMover.h"
#include "../Window.h"
#include "../util/ModeEnum.h"
#include "../rendering/Renderer.h"

namespace vOS
{
    class MeshView: public WindowPanel
    {
    public:

        MeshView(int width, int height);
        void show() override;
        void m_take_screenshot(const std::string& filename);

    private:

        void handleResize();
        void render_debug_menu();
        void querySelection(int type, int picked_id);
        [[nodiscard]] unsigned int get_selected_texture();

        // these are just to differentiate between the different textures, we cannot directly use the texture ids
        // since they can change when resizing the framebuffer
        static const int FINAL_IMAGE            = 0;
        static const int SELECTION              = 1;
        static const int SSAO_PRE               = 2;
        static const int SSAO_BLUR              = 3;
        static const int TRANSPARENCY_ACCUM     = 4;
        static const int TRANSPARENCY_REVEAL    = 5;
        static const int SHADOW_MAP             = 6;

        static const int SELECTION_TYPE_VERTEX = 1;
        static const int SELECTION_TYPE_EDGE = 2;
        static const int SELECTION_TYPE_FACE = 3;

        int m_hovered_element_id = 0;
        int m_hovered_element_type = 0;

        bool m_lastDown;
        double m_lastX;
        double m_lastY;

        int m_viewportPanelWidth;
        int m_viewportPanelHeight;

        // opengl rendering
        std::shared_ptr<Renderer> m_renderer = nullptr;
        std::shared_ptr<FrameBufferObject> m_meshFrameBuffer = nullptr;
        std::shared_ptr<FrameBufferObject> m_screen_quad_frameBuffer = nullptr;
        RenderData m_render_data;

        // selected texture for rendering (mostly for debugging)
        int m_viewport_texture = FINAL_IMAGE;
        int m_transparency = WEIGHTED_BLENDED;
        int m_shadow_map_cascade_level_debug = 0;

        // Other Rendering Components
        MeshMover m_mover;

        friend class Window;
    };
}