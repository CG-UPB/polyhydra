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

        [[nodiscard]] const glm::vec4& get_base_color() const
        {
            return m_base_color;
        }

        void set_base_color(float r, float g, float b)
        {
            m_base_color.x = r;
            m_base_color.y = g;
            m_base_color.z = b;
        }

        [[nodiscard]] const glm::mat4& get_transform()
        {
            if (m_transform_changed)
            {
                glm::mat4 position = glm::translate(m_position);
                glm::mat4 scale = glm::scale(m_scale);
                glm::mat4 rotation = glm::rotate(m_rotation.x, glm::vec3(m_rotation.y, m_rotation.z, m_rotation.w));
                m_transform = position * rotation * scale;
            }
            return m_transform;
        }

        void set_position(float x, float y, float z)
        {
            m_position.x = x;
            m_position.y = y;
            m_position.z = z;
            m_transform_changed = true;
        }

        [[nodiscard]] const glm::vec3& get_position() const
        {
            return m_position;
        }

        void set_scale(float x, float y, float z)
        {
            m_scale.x = x;
            m_scale.y = y;
            m_scale.z = z;
            m_transform_changed = true;
        }

        [[nodiscard]] const glm::vec3& get_scale() const
        {
            return m_scale;
        }

        [[nodiscard]] bool is_visible() const
        {
            return m_visible;
        }

        void set_visible(bool visible)
        {
            m_visible = visible;
        }

        [[nodiscard]] Shader* get_shader() const
        {
            return m_shader;
        }

        int get_id(){return id;}
        void set_id(int _id){id = _id;}

    protected:

        Shader* m_shader = Shader::basic_shape_shader();

    private:
        int id = 0;

        bool m_visible = true;
        glm::vec4 m_base_color = glm::vec4(1.0f);
        glm::mat4 m_transform = glm::mat4(1.0f);
        glm::vec3 m_position = glm::vec3(0.0f);
        glm::vec3 m_scale = glm::vec3(1.0f);
        glm::vec4 m_rotation = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        bool m_transform_changed = false;
    };
}