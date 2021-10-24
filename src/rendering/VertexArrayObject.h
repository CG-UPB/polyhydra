#pragma once

#include <vector>

namespace vOS
{
    class VertexArrayObject
    {
    public:
        VertexArrayObject(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
        ~VertexArrayObject();
        void update(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
        void draw();

    private:

        unsigned int m_vao;
        unsigned int m_vbo;
        unsigned int m_ibo;

        int m_numIndices;
    };
}