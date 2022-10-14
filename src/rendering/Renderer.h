#pragma once

#include "vospch.h"

#include "gl/FrameBufferObject.h"
#include "../rendering/gl/PixelBufferObject.h"
#include "../rendering/gl/PrePassFrameBufferObject.h"
#include "../rendering/passes/MeshPass.h"
#include "../rendering/passes/SSAOPass.h"
#include "../rendering/passes/BackgroundPass.h"
#include "../rendering/passes/GroundPass.h"
#include "../rendering/passes/SelectionPass.h"
#include "../rendering/passes/SelectionHoverPass.h"
#include "../rendering/passes/PrePass.h"
#include "../rendering/passes/ShadowMapPass.h"
#include "../rendering/passes/TransparencyPassWB.h"
#include "../rendering/passes/TransparencyPassDP.h"
#include "../rendering/passes/VertexOnlyPass.h"
#include "../rendering/passes/PostProcessingPass.h"
#include "../rendering/passes/OutlinePass.h"
#include "../rendering/shapes/ShapeRenderer.h"
#include "../mesh/MeshList.h"
#include "camera/Camera.h"
#include "Light.h"

namespace volumeshOS
{
    struct ExportOptions;
}

namespace volumeshOS::Internal
{
    class SSAOPass;
    class TransparencyPassWB;
    class TransparencyPassDP;
    class ShadowMapPass;
    class PostProcessingPass;
    class OutlinePass;

    struct RenderData
    {
        bool render_bg          = true;
        bool render_shapes      = true;
        bool update_input       = true;
        bool render_ground      = true;
        bool ground_shadow_only = false;
    };

    class Renderer
    {
    public:

        Renderer(int width, int height);

        void set_selection_callback(std::function<void(int, int)> callback) { selection_callback = std::move(callback); };

        void resize(int width, int height);

        void render(const RenderData& data = {});

        void export_image(const std::string& path, const ExportOptions& options);

    public:

        [[nodiscard]] static bool should_render_mesh(const std::shared_ptr<MeshObject>& mesh);

    public:

        std::shared_ptr<MeshList> mesh_list     = nullptr;
        std::shared_ptr<Camera> camera          = nullptr;
        std::shared_ptr<ShapeRenderer> shapes   = nullptr;
        std::vector<std::shared_ptr<MeshObject>> render_list;
        std::function<void(int, int)> selection_callback;
        glm::vec3 hover_position = {0.0, 0.0, 0.0};
        bool input_blocking                     = false;

        // render buffers
        struct
        {
            std::shared_ptr<FrameBufferObject> target_framebuffer_ms    = nullptr;
            std::shared_ptr<FrameBufferObject> target_framebuffer       = nullptr;
            std::shared_ptr<FrameBufferObject> post_framebuffer         = nullptr;
            std::shared_ptr<FrameBufferObject> selection_frame_buffer   = nullptr;
            std::shared_ptr<PixelBufferObject> pixel_buffer             = nullptr;
        } buffers;

        // render passes
        struct
        {
            std::shared_ptr<BackgroundPass> background_pass             = nullptr;
            std::shared_ptr<GroundPass> ground_pass                     = nullptr;
            std::shared_ptr<PrePass> pre_pass                           = nullptr;
            std::shared_ptr<ShadowMapPass> shadow_pass                  = nullptr;
            std::shared_ptr<MeshPass> mesh_pass                         = nullptr;
            std::shared_ptr<SSAOPass> ssao_pass                         = nullptr;
            std::shared_ptr<TransparencyPassWB> transparency_pass_wb    = nullptr;
            std::shared_ptr<TransparencyPassDP> transparency_pass_dp    = nullptr;
            std::shared_ptr<SelectionPass> selection_pass               = nullptr;
            std::shared_ptr<SelectionHoverPass> selection_hover_pass    = nullptr;
            std::shared_ptr<VertexOnlyPass> vertex_only_pass            = nullptr;
            std::shared_ptr<PostProcessingPass> post_processing_pass    = nullptr;
            std::shared_ptr<OutlinePass> outline_pass                   = nullptr;
        } passes;

        // frame attributes
        struct
        {
            int limit                       = 4;
            int current                     = 0;
            int width                       = 0;
            int height                      = 0;
            bool is_rendering_background    = false;
            bool ground_shadow_only         = false;
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

        void handle_zoom();
        void handle_input();
        void handle_camera_input();
        void handle_mesh_input();
    };
}