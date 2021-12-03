#pragma once

#include <vector>
#include <unordered_map>

namespace vOS
{
    class IcoSphereMesh
    {

    public:

        explicit IcoSphereMesh(int recursion_level);
        const std::vector<float>& vertices();
        const std::vector<unsigned int>& indices();
        const std::vector<float>& normals();

    private:

        struct Face
        {
            unsigned int v0;
            unsigned int v1;
            unsigned int v2;
        };

        unsigned int get_mid_point(unsigned int v0, unsigned int v1);
        unsigned int add_vertex(float x, float y, float z);

        unsigned int m_index;
        std::vector<float> m_vertices;
        std::vector<unsigned int> m_indices;
        std::vector<float> m_normals;
        std::unordered_map<long, unsigned int> m_mid_point_cache;
    };
}
