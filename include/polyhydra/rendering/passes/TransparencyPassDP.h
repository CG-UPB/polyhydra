#pragma once

#include "polyhydra/rendering/Renderer.h"
#include "polyhydra/rendering/passes/RenderPass.h"

namespace polyhydra::Internal
{
class Renderer;

class TransparencyPassDP : public RenderPass
{
  public:
    explicit TransparencyPassDP(int width, int height);
    ~TransparencyPassDP();

    void render(const Renderer& renderer) override;
    void render_mesh(const Renderer& renderer, const std::shared_ptr<MeshObject>& mesh, int layer);
    void render_composition(const Renderer& renderer, int current_passes, int max_passes, uint32_t tex);

    void resize_buffers(int width, int height);

    int m_width;
    int m_height;

    std::shared_ptr<FrameBufferObject> m_transparent_framebuffer0;
    std::shared_ptr<FrameBufferObject> m_transparent_framebuffer1;

  private:
    std::shared_ptr<FrameBufferObject> m_opaque_depth_framebuffer;

    void release_render_target_resources();

    void create_peel_framebuffers();

    void ensure_layer_textures(int num_passes);

    std::vector<uint32_t> m_layer_textures;
    int m_layer_width = 0;
    int m_layer_height = 0;

    bool m_copy_opaque_depth = false;

    std::shared_ptr<Shader> m_transparency_shader;
    std::shared_ptr<Shader> m_composite_shader;

    float m_alpha_pow = 1.0f;
    float m_pow = 1.0f;
    float m_range = 0.3f;
    float m_depth_range = 200.0f;
    float m_ordering_strength = 4.0f;
    float m_min = 0.01f;
    float m_max = 3000.0f;

    bool m_cullface = true;
};
} // namespace polyhydra::Internal