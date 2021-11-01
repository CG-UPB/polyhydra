#pragma once

#include <vector>

namespace vOS
{
    class VertexArrayObject
    {
    public:
        VertexArrayObject(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
        VertexArrayObject(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, const std::vector<float>& texture_coordinates);
        ~VertexArrayObject();
        void update(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
        void draw();

    private:

        unsigned int m_vao;
        unsigned int m_vbo;
        unsigned int m_ibo;

        // optional
        unsigned int m_tbo = -1;

        int m_numIndices;
    };
}