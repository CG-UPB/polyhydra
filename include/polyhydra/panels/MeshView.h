#pragma once

#include "polyhydra/polyhydraPCH.h"

#include "polyhydra/panels/LogWindow.h"
#include "polyhydra/panels/WindowPanel.h"
#include "polyhydra/rendering/Renderer.h"
#include "polyhydra/rendering/gl/PixelBufferObject.h"
#include "polyhydra/rendering/gl/PrePassFrameBufferObject.h"
#include "polyhydra/rendering/passes/BackgroundPass.h"
#include "polyhydra/rendering/passes/MeshPass.h"
#include "polyhydra/rendering/passes/PrePass.h"
#include "polyhydra/rendering/passes/SSAOPass.h"
#include "polyhydra/rendering/passes/SelectionHoverPass.h"
#include "polyhydra/rendering/passes/SelectionPass.h"
#include "polyhydra/rendering/passes/ShadowMapPass.h"
#include "polyhydra/rendering/passes/TransparencyPassDP.h"
#include "polyhydra/rendering/passes/TransparencyPassWB.h"
#include "polyhydra/rendering/passes/VertexOnlyPass.h"
#include "polyhydra/util/Enums.h"

namespace polyhydra::Internal
{
class MeshView : public WindowPanel
{
  public:
    MeshView(int width, int height);
    void show() override;

  public:
    std::shared_ptr<Renderer> renderer = nullptr;
    std::shared_ptr<LogWindow> log_window = nullptr;

  private:
    void handle_resize();
    void render_debug_menu();
    void handle_mouse_hover(int type, int picked_id);
    [[nodiscard]] uint32_t get_selected_texture();

  private:
    void handle_vertex_hover(const std::shared_ptr<MeshObject>& mesh, OpenVolumeMesh::VertexHandle vertex);
    void handle_edge_hover(const std::shared_ptr<MeshObject>& mesh, OpenVolumeMesh::EdgeHandle edge);
    void handle_halfface_hover(const std::shared_ptr<MeshObject>& mesh, OpenVolumeMesh::HalfFaceHandle halfface);

  private:
    // these are just to differentiate between the different textures, we cannot directly use the texture ids
    // since they can change when resizing the framebuffer
    static constexpr const int FINAL_IMAGE = 0;
    static constexpr const int SELECTION = 1;
    static constexpr const int SSAO_PRE = 2;
    static constexpr const int SSAO_BLUR = 3;
    static constexpr const int TRANSPARENCY_ACCUM = 4;
    static constexpr const int TRANSPARENCY_REVEAL = 5;
    static constexpr const int SHADOW_MAP = 6;

    // selection types
    static constexpr const int SELECTION_TYPE_NONE = 0;
    static constexpr const int SELECTION_TYPE_VERTEX = 1;
    static constexpr const int SELECTION_TYPE_EDGE = 2;
    static constexpr const int SELECTION_TYPE_HALFFACE = 3;
    static constexpr const int SELECTION_TYPE_CELL = 4;
    static constexpr const char* SELECTION_TYPE_NAME[] = {"None", "Vertex", "Edge", "Halfface", "Cell"};

    // window dimensions
    int m_viewport_panel_width = 0;
    int m_viewport_panel_height = 0;

    // selected texture for rendering (mostly for debugging)
    int m_viewport_texture = FINAL_IMAGE;
    int m_shadow_map_cascade_level_debug = 0;

    // store current selection attributes
    int m_hovered_element_type = SELECTION_TYPE_NONE;
    int m_hovered_element_ovm_id = -1;

    friend class Window;
};
} // namespace polyhydra::Internal