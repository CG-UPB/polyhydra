#pragma once

#include "RenderPass.h"

namespace vOS
{
    class BackgroundPass : public RenderPass
    {
    public:
        BackgroundPass();
        ~BackgroundPass();

        void set_background_color(const glm::vec4& color);
        [[nodiscard]] const glm::vec4& get_background_color() const;

        void render(VertexArrayObject* vao, const RenderData &data) override;

    private:

        glm::vec4 m_top_color;
        glm::vec4 m_bottom_color;

        VertexArrayObject* m_vao;
        Shader* m_background_shader;
    };
}