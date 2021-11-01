
#include <glad/glad.h>

#include "VertexArrayObject.h"


#include <iostream>

namespace vOS {

    VertexArrayObject::VertexArrayObject(const std::vector<float> &vertices, const std::vector<unsigned int> &indices) {
        m_numIndices = (int) indices.size();



        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, (int) vertices.size() * 4, vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &m_ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (int) indices.size() * 4, indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    VertexArrayObject::VertexArrayObject(const std::vector<float>& vertices, const std::vector<unsigned int>& indices,
                                         const std::vector<float>& texture_coordinates): VertexArrayObject(vertices, indices)
    {
        glBindVertexArray(m_vao);

        glGenBuffers(1, &m_tbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_tbo);
        glBufferData(GL_ARRAY_BUFFER, (int) texture_coordinates.size() * 4, texture_coordinates.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    VertexArrayObject::~VertexArrayObject()
    {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ibo);

        if (m_tbo != -1)
        {
            glDeleteBuffers(1, &m_tbo);
        }
    }

    void VertexArrayObject::draw()
    {
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void VertexArrayObject::update(const std::vector<float> &vertices, const std::vector<unsigned int> &indices)
    {
        m_numIndices = (int) indices.size();
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, (int) vertices.size() * 4, vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (int) indices.size() * 4, indices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}