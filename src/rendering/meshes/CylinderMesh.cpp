
#include "polyhydra/rendering/meshes/CylinderMesh.h"

namespace polyhydra::Internal
{
CylinderMesh::CylinderMesh(int samples, float radius, float height)
{
    std::vector<float> circle_vertices_2d;
    float half_height = height * 0.5f;
    float step_size = (float)(2.0 * PI) / (float)samples;
    for (int i = 0; i < samples; i++)
    {
        float angle = (float)i * step_size;
        circle_vertices_2d.push_back(std::cos(angle) * radius);
        circle_vertices_2d.push_back(std::sin(angle) * radius);
    }

    int top_start = 0;
    for (int i = 0; i < samples; i++)
    {
        // top circle side
        float angle = (float)i * step_size;
        add_vertex(circle_vertices_2d[i * 2 + 0], half_height, circle_vertices_2d[i * 2 + 1]);
        add_normal(std::cos(angle), 0.0f, std::sin(angle));
    }

    int bottom_start = samples;
    for (int i = 0; i < samples; i++)
    {
        // bottom circle side
        float angle = (float)i * step_size;
        add_vertex(circle_vertices_2d[i * 2 + 0], -half_height, circle_vertices_2d[i * 2 + 1]);
        add_normal(std::cos(angle), 0.0f, std::sin(angle));
    }

    // side indices
    for (int i = 0; i < samples; i++)
    {
        add_triangle(bottom_start + i, bottom_start + (i + 1) % samples, top_start + (i + 1) % samples);
        add_triangle(top_start + (i + 1) % samples, top_start + i, bottom_start + i);
    }
}

void CylinderMesh::add_vertex(float x, float y, float z)
{
    m_vertices.push_back(x);
    m_vertices.push_back(y);
    m_vertices.push_back(z);
}

void CylinderMesh::add_triangle(int v0, int v1, int v2)
{
    m_indices.push_back(v0);
    m_indices.push_back(v1);
    m_indices.push_back(v2);
}

void CylinderMesh::add_normal(float x, float y, float z)
{
    m_normals.push_back(x);
    m_normals.push_back(y);
    m_normals.push_back(z);
}

const std::vector<float>& CylinderMesh::vertices()
{
    return m_vertices;
}

const std::vector<uint32_t>& CylinderMesh::indices()
{
    return m_indices;
}

const std::vector<float>& CylinderMesh::normals()
{
    return m_normals;
}
} // namespace polyhydra::Internal