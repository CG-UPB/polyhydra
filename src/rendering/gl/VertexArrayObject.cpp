
#include "glad/glad.h"

#include "VertexArrayObject.h"

#include <iostream>
#include <type_traits>
#include "../meshes/CommonMeshes.h"

namespace vOS
{
    VertexArrayObject* VertexArrayObject::s_screen_quad = nullptr;

    void VertexArrayObject::draw_screen_quad()
    {
        if (s_screen_quad == nullptr)
        {
            s_screen_quad = new VertexArrayObject(CommonMeshes::PlaneXY::vertices(2.0f, 2.0f),
                                              CommonMeshes::PlaneXY::indices());
            s_screen_quad->add_attribute(CommonMeshes::PlaneXY::uvs(), 1, 2);
        }
        s_screen_quad->draw();
    }

    void VertexArrayObject::clean_up()
    {
        delete s_screen_quad;
    }

    VertexArrayObject::VertexArrayObject(const std::vector<float>& vertices, const std::vector<unsigned int>& indices)
    {
        m_numIndices = (int) indices.size();

        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, (int) vertices.size() * 4, vertices.data(), GL_DYNAMIC_DRAW);

        glGenBuffers(1, &m_ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (int) indices.size() * 4, indices.data(), GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    VertexArrayObject::~VertexArrayObject()
    {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ibo);

        for (unsigned int buffer: m_buffers)
        {
            glDeleteBuffers(1, &buffer);
        }
    }

    void VertexArrayObject::draw() const
    {
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void VertexArrayObject::draw_instanced(int num_instances) const
    {
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glDrawElementsInstanced(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, nullptr, num_instances);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void VertexArrayObject::update_vertices(const std::vector<float>& vertices, const std::vector<unsigned int>& indices)
    {
        m_numIndices = (int) indices.size();
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, (int) vertices.size() * 4, vertices.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (int) indices.size() * 4, indices.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    template<typename T>
    void VertexArrayObject::add_attribute(const std::vector<T>& data, int location, int element_count, bool per_instance)
    {
        int gl_type;
        if constexpr(std::is_same_v<T, float>)
        { gl_type = GL_FLOAT; }
        else if constexpr(std::is_same_v<T, int>)
        { gl_type = GL_INT; }
        else if constexpr(std::is_same_v<T, unsigned int>)
        { gl_type = GL_UNSIGNED_INT; }
        else
        { throw std::invalid_argument("Invalid data type for gl buffer"); }

        m_location_buffer_index[location] = (int) m_buffers.size();
        m_buffers.push_back(-1);

        glBindVertexArray(m_vao);
        glGenBuffers(1, &m_buffers[m_buffers.size() - 1]);
        glBindBuffer(GL_ARRAY_BUFFER, m_buffers[m_buffers.size() - 1]);
        glBufferData(GL_ARRAY_BUFFER, (int) data.size() * sizeof(T), data.data(), GL_DYNAMIC_DRAW);
        if (gl_type == GL_FLOAT)
        {
            glVertexAttribPointer(location, element_count, gl_type, GL_FALSE, element_count * sizeof(T), nullptr);
        }
        else
        {
            glVertexAttribIPointer(location, element_count, gl_type, element_count * sizeof(T), nullptr);
        }
        glEnableVertexAttribArray(location);
        if (per_instance)
        {
            glVertexAttribDivisor(location, 1);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    template<typename T>
    void VertexArrayObject::update_attribute(const std::vector<T>& data, int location)
    {
        int buffer_index = m_location_buffer_index[location];
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_buffers[buffer_index]);
        glBufferData(GL_ARRAY_BUFFER, (int) data.size() * sizeof(T), data.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    template<typename T>
    void VertexArrayObject::update_attribute(const std::vector<T>& data, int location, int offset, int size)
    {
        int buffer_index = m_location_buffer_index[location];
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_buffers[buffer_index]);
        glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(T), (int) size * sizeof(T), &data[offset]);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    // these are necessary for the linker to find the templated function types, otherwise we would need to implement
    // the template function in the header file, which would cause some problems
    template void VertexArrayObject::add_attribute<float>(const std::vector<float>& data, int location, int element_count, bool per_instance = false);
    template void VertexArrayObject::add_attribute<int>(const std::vector<int>& data, int location, int element_count, bool per_instance = false);
    template void VertexArrayObject::add_attribute<unsigned int>(const std::vector<unsigned int>& data, int location, int element_count, bool per_instance = false);

    template void VertexArrayObject::update_attribute<float>(const std::vector<float>& data, int location);
    template void VertexArrayObject::update_attribute<int>(const std::vector<int>& data, int location);
    template void VertexArrayObject::update_attribute<unsigned int>(const std::vector<unsigned int>& data, int location);

    template void VertexArrayObject::update_attribute<float>(const std::vector<float>& data, int location, int offset, int size);
    template void VertexArrayObject::update_attribute<int>(const std::vector<int>& data, int location, int offset, int size);
    template void VertexArrayObject::update_attribute<unsigned int>(const std::vector<unsigned int>& data, int location, int offset, int size);
}