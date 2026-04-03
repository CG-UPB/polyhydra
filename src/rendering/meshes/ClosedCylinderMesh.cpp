
#include "polyhydra/rendering/meshes/ClosedCylinderMesh.h"

namespace polyhydra::Internal
{
ClosedCylinderMesh::ClosedCylinderMesh(int samples, float radius, float height)
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

    // top and bottom circle center
    add_vertex(0.0f, half_height, 0.0f);
    add_normal(0.0f, 1.0f, 0.0f);

    add_vertex(0.0f, -half_height, 0.0f);
    add_normal(0.0f, -1.0f, 0.0f);

    int off = 2;
    for (int i = 0; i < samples; i++)
    {
        // top circle
        add_vertex(circle_vertices_2d[i * 2 + 0], half_height, circle_vertices_2d[i * 2 + 1]);
        add_normal(0.0f, 1.0f, 0.0f);
        add_triangle(off + i, off + (i + 1) % samples, 0);
    }
    off += samples;

    for (int i = 0; i < samples; i++)
    {
        // bottom circle
        add_vertex(circle_vertices_2d[i * 2 + 0], -half_height, circle_vertices_2d[i * 2 + 1]);
        add_normal(0.0f, -1.0f, 0.0f);
        add_triangle(off + i, off + (i + 1) % samples, 1);
    }
    off += samples;

    int top_start = off;
    for (int i = 0; i < samples; i++)
    {
        // top circle side
        float angle = (float)i * step_size;
        add_vertex(circle_vertices_2d[i * 2 + 0], half_height, circle_vertices_2d[i * 2 + 1]);
        add_normal(std::cos(angle), 0.0f, std::sin(angle));
    }
    off += samples;

    int bottom_start = off;
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

void ClosedCylinderMesh::add_vertex(float x, float y, float z)
{
    m_vertices.push_back(x);
    m_vertices.push_back(y);
    m_vertices.push_back(z);
}

void ClosedCylinderMesh::add_triangle(int v0, int v1, int v2)
{
    m_indices.push_back(v0);
    m_indices.push_back(v1);
    m_indices.push_back(v2);
}

void ClosedCylinderMesh::add_normal(float x, float y, float z)
{
    m_normals.push_back(x);
    m_normals.push_back(y);
    m_normals.push_back(z);
}

const std::vector<float>& ClosedCylinderMesh::vertices()
{
    return m_vertices;
}

const std::vector<uint32_t>& ClosedCylinderMesh::indices()
{
    return m_indices;
}

const std::vector<float>& ClosedCylinderMesh::normals()
{
    return m_normals;
}
} // namespace polyhydra::Internal