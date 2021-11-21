#pragma once

#include <vector>
#include <string>

namespace vOS
{
    class VertexArrayObject
    {
    public:

        VertexArrayObject(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);

        ~VertexArrayObject();

        void update_vertices(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);

        void draw() const;

        template<typename T>
        void add_buffer(const std::vector<T>& data, int location, int element_count);

        template<typename T>
        void update_buffer(const std::vector<T>& data, int location);

    private:

        int m_numIndices;

        unsigned int m_vao;
        unsigned int m_vbo;
        unsigned int m_ibo;

        // optional additional buffers
        std::vector<unsigned int> m_buffers;
    };
}