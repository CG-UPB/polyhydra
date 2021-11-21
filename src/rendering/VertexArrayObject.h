#pragma once

#include <vector>
#include <string>

namespace vOS
{
    class VertexArrayObject
    {
    public:
        VertexArrayObject(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
        VertexArrayObject(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, const std::vector<float>& coordinates, std::string id);
        ~VertexArrayObject();
        void update(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
        void draw();

    private:

        unsigned int m_vao;
        unsigned int m_vbo;
        unsigned int m_ibo;

        // optional
        unsigned int m_tbo = -1;
        unsigned int m_nbo = -1;

        int m_numIndices;
    };
}