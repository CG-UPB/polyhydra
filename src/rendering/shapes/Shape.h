#pragma once

#include "vospch.h"

#include "../gl/Shader.h"
#include "../gl/VertexArrayObject.h"

namespace volumeshOS
{
    /**
     * This class is the super-class for the shapes. If an user wants to create new shapes he easily overrides this class
     * and can use its shape in volumeshOS
     */
    class Shape
    {
    public:

        virtual ~Shape() = default;

        /**
         * draw method is the method, that will be called to draw the shape
         */
        virtual void draw() = 0;

        /**
         * Getter for the base_color
         * @return base_color
         */
        [[nodiscard]] const glm::vec4& get_base_color() const
        {
            return m_base_color;
        }

        /**
         * Setter for the base_color
         * @param base_color in r,g.b
         */
        void set_base_color(float r, float g, float b)
        {
            m_base_color.x = r;
            m_base_color.y = g;
            m_base_color.z = b;
        }

        /**
         * method to get the transform matrix of the shape
         *
         * @return glm::mat4 as the transform-matrix
         */
        [[nodiscard]] const glm::mat4& get_transform()
        {
            if (m_transform_changed)
            {
                glm::mat4 position = glm::translate(m_position);
                glm::mat4 scale = glm::scale(m_scale);
                glm::mat4 rotation = glm::rotate(m_rotation.x, glm::vec3(m_rotation.y, m_rotation.z, m_rotation.w));
                m_transform = position * rotation * scale;
                m_transform_changed = false;
            }
            return m_transform;
        }


        /**
         *
         * sets the position of the shape
         *
         * @param x coordinate
         * @param y coordinate
         * @param z coordinate
         */
        void set_position(float x, float y, float z)
        {
            m_position.x = x;
            m_position.y = y;
            m_position.z = z;
            m_transform_changed = true;
        }

        /**
         *
         * sets the position of the shape
         *
         * @return glm::vec3 as the position of the shape
         */
        [[nodiscard]] const glm::vec3& get_position() const
        {
            return m_position;
        }

        /**
         *
         * sets the scale of the shape
         *
         * @param x scale
         * @param y scale
         * @param z scale
         */
        void set_scale(float x, float y, float z)
        {
            m_scale.x = x;
            m_scale.y = y;
            m_scale.z = z;
            m_transform_changed = true;
        }


        /**
         *
         * gets the scale of the shape
         *
         * @return glm::vec3 as the scale of the shape
         */
        [[nodiscard]] const glm::vec3& get_scale() const
        {
            return m_scale;
        }

        /**
         * gets the visibility of the shape
         *
         * @return true, if shape is visible, else false
         */
        [[nodiscard]] bool is_visible() const
        {
            return m_visible;
        }

        /**
         * gets the visibility of the shape
         *
         * @param visible true if the shape should be visible, false else
         */
        void set_visible(bool visible)
        {
            m_visible = visible;
        }

        /**
         * gets the shader for the shape
         *
         * @return Shader* shader of the shape
         */
        [[nodiscard]] std::shared_ptr<Shader> get_shader() const
        {
            return m_shader;
        }

        /**
         * gets the shape_id
         *
         * @return shape_id
         */
        int get_id(){return id;}

        /**
         * sets the shape_id
         *
         * @param _id of the shape
         */
        void set_id(int _id){id = _id;}

    protected:

        // default shader
        std::shared_ptr<Shader> m_shader = Shader::basic_shape_shader();

    private:

        int id = 0;

        bool m_visible = true;

        // transformation vectors for the shape
        glm::vec4 m_base_color = glm::vec4(1.0f);
        glm::mat4 m_transform = glm::mat4(1.0f);
        glm::vec3 m_position = glm::vec3(0.0f);
        glm::vec3 m_scale = glm::vec3(1.0f);
        glm::vec4 m_rotation = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        bool m_transform_changed = false;
    };
}