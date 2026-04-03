#pragma once

#include "polyhydra/mesh/MeshObject.h"
#include "polyhydra/rendering/passes/RenderPass.h"

namespace polyhydra::Internal
{
/**
 * Renders the entire scene again in which every element of every mesh is rendered in a unique
 * color according to its ID. The color can then be reversed back to an ID to make determining which element the user is
 * hovering above easy to do
 */
class SelectionPass : public RenderPass
{
  public:
    SelectionPass();

    ~SelectionPass() = default;
    void render(const Renderer& renderer) override;

    /**
     * Renders specific Mesh
     * @param mesh
     * @param data
     * @param mesh_id
     */
    void render_mesh(const std::shared_ptr<MeshObject>& mesh, const Renderer& renderer);
    void set_debug_mode(bool mode);
    [[nodiscard]] bool is_debug_mode() const;

  private:
    bool m_debug = false;

    // Shaders
    std::shared_ptr<Shader> m_selection_shader;
    std::shared_ptr<Shader> m_selection_sphere_shader;
    std::shared_ptr<Shader> m_selection_cylinder_shader;

    // Mesh Information
    int m_num_vertices;
    int m_num_edges;
    // VAOs
    std::shared_ptr<VertexArrayObject> m_sphere_vao;
    std::shared_ptr<VertexArrayObject> m_cylinder_vao;
};
} // namespace polyhydra::Internal
