#pragma once

#include "RenderPass.h"

namespace volumeshOS::Internal
{
    /**
     * Renders the Background visible when no other Mesh occupies the Viewfield
     */
    class BackgroundPass : public RenderPass
    {
    public:

        BackgroundPass();

        /**
         * Sets the background color
         * @param color
         */
        void set_background_color(const glm::vec4& color);
        /**
         * Returns current background color
         * @return
         */
        [[nodiscard]] const glm::vec4& get_background_color() const;

        void render(const Renderer& renderer) override;

    private:

        // Rendering Variables

        // Colors
        glm::vec4 m_top_color;
        glm::vec4 m_bottom_color;
        // A simple plane mesh that is always visible but behind all other objects
        std::unique_ptr<VertexArrayObject> m_vao;
        // Simple gradient shader
        std::shared_ptr<Shader> m_background_shader;
    };
}