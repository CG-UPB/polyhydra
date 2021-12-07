#pragma once

#include <vector>

namespace vOS
{
    class CylinderMesh
    {

    public:

        explicit CylinderMesh(int samples = 10, float radius = 1.0f, float height = 1.0f);
        ~CylinderMesh() = default;

        const std::vector<float>& vertices();
        const std::vector<unsigned int>& indices();
        const std::vector<float>& normals();

    private:

        void add_vertex(float x, float y, float z);
        void add_normal(float x, float y, float z);
        void add_triangle(int v0, int v1, int v2);

        std::vector<float> m_vertices;
        std::vector<unsigned int> m_indices;
        std::vector<float> m_normals;
    };
}