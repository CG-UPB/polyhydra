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
#include "../rendering/passes/TransparencyPassWB.h"
#include "../rendering/passes/TransparencyPassDP.h"
#include "../rendering/passes/VertexOnlyPass.h"
#include "../rendering/passes/ShapePass.h"
#include "../mesh/MeshList.h"
#include "camera/Camera.h"
#include "Light.h"

namespace volumeshOS::Internal
{
    class MeshPass;
    class SSAOPass;
    class ShadowMapPass;
    class TransparencyPassWB;
    class TransparencyPassDP;

    class Renderer
    {
    public:

        Renderer(int width, int height, const std::shared_ptr<FrameBufferObject>& initial_target_ms, const std::shared_ptr<FrameBufferObject>& initial_target);

        void set_selection_callback(std::function<void(int, int)> callback) { m_selection_callback = std::move(callback); };

        void resize(int width, int height);

        void render(bool render_bg = true);

        void set_target_framebuffer(std::shared_ptr<FrameBufferObject> target_ms, std::shared_ptr<FrameBufferObject> target);

    public:

        std::shared_ptr<MeshList> mesh_list     = nullptr;
        std::shared_ptr<Camera> camera          = nullptr;
        Light light;
        std::vector<MeshObject&> render_list    = nullptr;

        // render buffers
        struct
        {
            std::shared_ptr<FrameBufferObject> target_framebuffer_ms    = nullptr;
            std::shared_ptr<FrameBufferObject> target_framebuffer       = nullptr;
            std::shared_ptr<FrameBufferObject> selection_frame_buffer   = nullptr;
            std::shared_ptr<PixelBufferObject> pixel_buffer             = nullptr;
        } buffers;

        // render passes
        struct
        {
            std::shared_ptr<BackgroundPass> background_pass             = nullptr;
            std::shared_ptr<PrePass> pre_pass                           = nullptr;
            std::shared_ptr<ShadowMapPass> shadow_pass                  = nullptr;
            std::shared_ptr<MeshPass> mesh_pass                         = nullptr;
            std::shared_ptr<SSAOPass> ssao_pass                         = nullptr;
            std::shared_ptr<TransparencyPassWB> transparency_pass_wb    = nullptr;
            std::shared_ptr<TransparencyPassDP> transparency_pass_dp    = nullptr;
            std::shared_ptr<ShapePass> shape_pass                       = nullptr;
            std::shared_ptr<SelectionPass> selection_pass               = nullptr;
            std::shared_ptr<SelectionHoverPass> selection_hover_pass    = nullptr;
            std::shared_ptr<VertexOnlyPass> vertex_only_pass            = nullptr;
        } passes;

        // frame attributes
        struct
        {
            int limit                       = 4;
            int current                     = 0;
            int width                       = 0;
            int height                      = 0;
            bool is_rendering_background    = false;
        } frame;

        // input handling
        struct
        {
            glm::vec2 last                  = {0.0f, 0.0f};
            glm::vec2 pos                   = {0.0f, 0.0f};
            glm::vec2 offset                = {0.0f, 0.0f};
            bool mesh_moving                = false;
        } input;

    private:

        void handle_input();
        void handle_camera_input();
        void handle_mesh_input();
        void render_mesh(const std::shared_ptr<MeshObject>& mesh);
        void render_selection();
        void render_pre_pass();
        void render_shadow_map();
        void render_ssao_pass();
        void render_background();
        void render_meshes();
        void render_transparency();
        void render_transparency_wb();
        void render_transparency_dp();
        void query_selection(int type, int id);

    private:

        // general
        GlobalViewerSettings& m_settings;
        std::function<void(int, int)> m_selection_callback;

        friend class Window;
        friend class MeshView;
        friend class MeshPass;
        friend class SSAOPass;
        friend class ShadowMapPass;
        friend class TransparencyPassWB;
        friend class TransparencyPassDP;
    };
}