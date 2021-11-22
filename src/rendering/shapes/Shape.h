#pragma once

#include "../Shader.h"
#include "../VertexArrayObject.h"
#include "glm/gtx/transform.hpp"

namespace vOS
{
    class Shape
    {
    public:

        virtual ~Shape() = default;
        virtual void draw() = 0;

        [[nodiscard]] const glm::vec4& get_base_color() const {
            return m_base_color;
        }

        void set_base_color(const glm::vec4& color) {
            m_base_color = color;
        }

        [[nodiscard]] const glm::mat4& get_transform() const {
            return m_transform;
        }

        void set_transform(const glm::mat4& transform) {
            m_transform = transform;
        }

        [[nodiscard]] bool is_visible() const {
            return m_visible;
        }

        void set_visible(bool visible) {
            m_visible = visible;
        }

        [[nodiscard]] Shader* get_shader() const {
            return m_shader;
        }

    protected:

        Shader* m_shader;

    private:

        bool m_visible = true;
        glm::vec4 m_base_color;
        glm::mat4 m_transform;
    };
}