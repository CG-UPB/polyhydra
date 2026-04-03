
#include "polyhydra/rendering/meshes/ConeMesh.h"

#include "polyhydra/util/VecUtil.h"

namespace polyhydra::Internal
{
ConeMesh::ConeMesh(int samples, float radius, float height)
{
    glm::vec3 tip_pos = {0.0f, height, 0.0f};
    std::vector<glm::vec3> circle_vertices;
    std::vector<glm::vec3> circle_normals;
    float step_size = (float)(2.0 * PI) / (float)samples;
    for (int i = 0; i < samples; i++)
    {
        float angle = (float)i * step_size;
        glm::vec3 position = {std::cos(angle) * radius, 0.0f, std::sin(angle) * radius};
        circle_vertices.emplace_back(position);

        auto tangent = position - tip_pos;
        glm::vec3 normal = glm::normalize(tip_pos + glm::dot(-tip_pos, glm::normalize(tangent)) * tangent);
        circle_normals.emplace_back(normal);
    }

    // base center (index = 1)
    add_vertex(0.0f, 0.0f, 0.0f);
    add_normal(0.0f, -1.0f, 0.0f);

    int off = 1;
    for (int i = 0; i < samples; i++)
    {
        // bottom circle
        auto position = circle_vertices[i];
        add_vertex(position.x, position.y, position.z);
        add_normal(0.0f, -1.0f, 0.0f);
        add_triangle(off + i, off + (i + 1) % samples, 1);
    }
    off += samples;

    int bottom_start = off;
    for (int i = 0; i < samples; i++)
    {
        // bottom side
        auto position = circle_vertices[i];
        auto normal = circle_normals[i];
        add_vertex(position.x, position.y, position.z);
        add_normal(normal.x, normal.y, normal.z);
    }
    off += samples;

    for (int i = 0; i < samples; i++)
    {
        // tip side
        auto normal = circle_normals[i];
        add_vertex(tip_pos.x, tip_pos.y, tip_pos.z);
        add_normal(normal.x, normal.y, normal.z);

        add_triangle(bottom_start + i, bottom_start + (i + 1) % samples, off + i);
    }
}

void ConeMesh::add_vertex(float x, float y, float z)
{
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
}

void ConeMesh::add_triangle(int v0, int v1, int v2)
{
    indices.push_back(v0);
    indices.push_back(v1);
    indices.push_back(v2);
}

void ConeMesh::add_normal(float x, float y, float z)
{
    normals.push_back(x);
    normals.push_back(y);
    normals.push_back(z);
}
} // namespace polyhydra::Internal