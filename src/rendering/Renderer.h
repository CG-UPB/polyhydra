#pragma once

#include "vospch.h"

#include "gl/FrameBufferObject.h"
#include "../rendering/gl/PixelBufferObject.h"
#include "../rendering/gl/PrePassFrameBufferObject.h"
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
#include "../Window.h"

namespace vOS
{
    class MeshPass;
    class SSAOPass;
    class ShadowMapPass;
    class ShadowColorFilterPass;
    class TransparentShadowMapPass;
    class TransparencyPass_WB;
    class TransparencyPass_DP;

    typedef std::function<void(int type, int picked_id)> SelectionCallback;

    class Renderer
    {
    public:

        Renderer(int width, int height, std::shared_ptr<FrameBufferObject> initial_target_ms, std::shared_ptr<FrameBufferObject> initial_target);

        void set_selection_callback(SelectionCallback callback) { m_selection_callback = std::move(callback); };

        void resize(int width, int height);

        void render(RenderData* render_data, bool render_bg = true);

        void set_target_framebuffer(std::shared_ptr<FrameBufferObject> target_ms, std::shared_ptr<FrameBufferObject> target);

    private:

        void render_mesh(RenderData& render_data, const std::shared_ptr<MeshObject>& mesh);
        void render_selection(RenderData& render_data);
        void render_pre_pass(RenderData& render_data);
        void render_shadow_map(RenderData& render_data);
        void render_ssao_pass(RenderData& render_data);
        void render_background(RenderData& render_data);
        void render_meshes(RenderData& render_data);
        void render_transparency(RenderData& render_data);
        void render_transparency_wb(RenderData& render_data);
        void render_transparency_dp(RenderData& render_data);
        void query_selection(int type, int id);

        // render buffers
        std::shared_ptr<FrameBufferObject> m_target_ms                          = nullptr;
        std::shared_ptr<FrameBufferObject> m_target                             = nullptr;
        std::shared_ptr<FrameBufferObject> m_selectionFrameBuffer               = nullptr;
        std::shared_ptr<PixelBufferObject> m_pixel_buffer                       = nullptr;

        // render passes
        std::shared_ptr<PrePass> m_pre_pass                                     = nullptr;
        std::shared_ptr<ShadowMapPass> m_shadow_pass                            = nullptr;
        std::shared_ptr<ShadowColorFilterPass> m_shadow_color_filter_pass       = nullptr;
        std::shared_ptr<TransparentShadowMapPass> m_transparent_shadow_pass     = nullptr;
        std::shared_ptr<MeshPass> m_mesh_pass                                   = nullptr;
        std::shared_ptr<SSAOPass> m_ssao_pass                                   = nullptr;
        std::shared_ptr<TransparencyPass_WB> m_transparency_pass_wb             = nullptr;
        std::shared_ptr<TransparencyPass_DP> m_transparency_pass_dp             = nullptr;
        BackgroundPass m_background_pass;
        ShapePass m_shape_pass;
        SelectionPass m_selection_pass;
        SelectionHoverPass m_selection_hover_pass;
        VertexOnlyPass m_vertex_only_pass;

        int m_frame_limit = 4;
        int m_current_frame = 0;

        int m_viewportPanelWidth;
        int m_viewportPanelHeight;

        bool m_is_rendering_background = false;

        RenderData* m_render_data = nullptr;

        GlobalViewerSettings& m_settings;

        SelectionCallback m_selection_callback;

        friend class Window;
        friend class MeshView;
        friend class MeshPass;
        friend class SSAOPass;
        friend class ShadowMapPass;
        friend class ShadowColorFilterPass;
        friend class TransparentShadowMapPass;
        friend class TransparencyPass_WB;
        friend class TransparencyPass_DP;
    };
}