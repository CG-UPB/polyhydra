
#include "CylinderMesh.h"

#include <cmath>

namespace vOS
{
    CylinderMesh::CylinderMesh(int samples, float radius, float height)
    {
        std::vector<float> circle_vertices_2d;
        float step_size = (float) (2.0 * M_PI) / (float) samples;
        for (int i = 0; i < samples; i++)
        {
            float angle = (float) i * step_size;
            circle_vertices_2d.push_back(std::cos(angle) * radius);
            circle_vertices_2d.push_back(std::sin(angle) * radius);
        }

        float half_height = height * 0.5f;

        // top and bottom circle center
        m_vertices.push_back(0.0f);
        m_vertices.push_back(0.0f);
        m_vertices.push_back(half_height);

        m_normals.push_back(0.0f);
        m_normals.push_back(0.0f);
        m_normals.push_back(1.0f);

        m_vertices.push_back(0.0f);
        m_vertices.push_back(0.0f);
        m_vertices.push_back(-half_height);

        m_normals.push_back(0.0f);
        m_normals.push_back(0.0f);
        m_normals.push_back(-1.0f);

        int off = 2;
        for (int i = 0; i < samples; i++)
        {
            // top circle
            m_vertices.push_back(circle_vertices_2d[i * 2 + 0]);
            m_vertices.push_back(circle_vertices_2d[i * 2 + 1]);
            m_vertices.push_back(half_height);

            m_normals.push_back(0.0f);
            m_normals.push_back(0.0f);
            m_normals.push_back(1.0f);

            m_indices.push_back(off + i);
            m_indices.push_back(off + (i + 1) % samples);
            m_indices.push_back(0);
        }
        off += samples;

        for (int i = 0; i < samples; i++)
        {
            // bottom circle
            m_vertices.push_back(circle_vertices_2d[i * 2 + 0]);
            m_vertices.push_back(circle_vertices_2d[i * 2 + 1]);
            m_vertices.push_back(-half_height);

            m_normals.push_back(0.0f);
            m_normals.push_back(0.0f);
            m_normals.push_back(-1.0f);

            m_indices.push_back(off + i);
            m_indices.push_back(off + (i + 1) % samples);
            m_indices.push_back(1);
        }
        off += samples;

        int top_side_start = off;
        for (int i = 0; i < samples; i++)
        {
            float angle = (float) i * step_size;

            // top circle side
            m_vertices.push_back(circle_vertices_2d[i * 2 + 0]);
            m_vertices.push_back(circle_vertices_2d[i * 2 + 1]);
            m_vertices.push_back(half_height);

            m_normals.push_back(std::cos(angle));
            m_normals.push_back(std::sin(angle));
            m_normals.push_back(0.0f);
        }
        off += samples;

        int bottom_side_start = off;
        for (int i = 0; i < samples; i++)
        {
            float angle = (float) i * step_size;

            // bottom circle side
            m_vertices.push_back(circle_vertices_2d[i * 2 + 0]);
            m_vertices.push_back(circle_vertices_2d[i * 2 + 1]);
            m_vertices.push_back(-half_height);

            m_normals.push_back(std::cos(angle));
            m_normals.push_back(std::sin(angle));
            m_normals.push_back(0.0f);
        }

        for (int i = 0; i < samples; i++)
        {
            m_indices.push_back(bottom_side_start + i);
            m_indices.push_back(bottom_side_start + (i + 1) % samples);
            m_indices.push_back(top_side_start + (i + 1) % samples);

            m_indices.push_back(top_side_start + (i + 1) % samples);
            m_indices.push_back(top_side_start + i);
            m_indices.push_back(bottom_side_start + i);
        }
    }

    const std::vector<float>& CylinderMesh::vertices()
    {
        return m_vertices;
    }

    const std::vector<unsigned int>& CylinderMesh::indices()
    {
        return m_indices;
    }

    const std::vector<float>& CylinderMesh::normals()
    {
        return m_normals;
    }
}