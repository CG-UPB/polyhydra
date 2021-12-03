
#include "IcoSphereMesh.h"
#include <cmath>

namespace vOS
{

    IcoSphereMesh::IcoSphereMesh(int recursion_level): m_index(0)
    {
        // create 12 vertices of the icosahedron
        float t = (1.0f + std::sqrt(5.0f)) * 0.5f;

        add_vertex(-1.0f,  t,  0.0f);
        add_vertex( 1.0f,  t,  0.0f);
        add_vertex(-1.0f, -t,  0.0f);
        add_vertex( 1.0f, -t,  0.0f);

        add_vertex( 0.0f, -1.0f,  t);
        add_vertex( 0.0f,  1.0f,  t);
        add_vertex( 0.0f, -1.0f, -t);
        add_vertex( 0.0f,  1.0f, -t);

        add_vertex( t,  0.0f, -1.0f);
        add_vertex( t,  0.0f,  1.0f);
        add_vertex(-t,  0.0f, -1.0f);
        add_vertex(-t,  0.0f,  1.0f);

        // create 20 triangles of the icosahedron
        std::vector<Face> faces;

        faces.push_back({0, 11, 5});
        faces.push_back({0, 5, 1});
        faces.push_back({0, 1, 7});
        faces.push_back({0, 7, 10});
        faces.push_back({0, 10, 11});

        faces.push_back({1, 5, 9});
        faces.push_back({5, 11, 4});
        faces.push_back({11, 10, 2});
        faces.push_back({10, 7, 6});
        faces.push_back({7, 1, 8});

        faces.push_back({3, 9, 4});
        faces.push_back({3, 4, 2});
        faces.push_back({3, 2, 6});
        faces.push_back({3, 6, 8});
        faces.push_back({3, 8, 9});

        faces.push_back({4, 9, 5});
        faces.push_back({2, 4, 11});
        faces.push_back({6, 2, 10});
        faces.push_back({8, 6, 7});
        faces.push_back({9, 8, 1});

        for (int i = 0; i < recursion_level; i++)
        {
            std::vector<Face> new_faces;
            for (const Face& face : faces)
            {
                // replace triangle by 4 triangles
                unsigned int a = get_mid_point(face.v0, face.v1);
                unsigned int b = get_mid_point(face.v1, face.v2);
                unsigned int c = get_mid_point(face.v2, face.v0);

                new_faces.push_back({face.v0, a, c});
                new_faces.push_back({face.v1, b, a});
                new_faces.push_back({face.v2, c, b});
                new_faces.push_back({a, b, c});
            }
            faces = new_faces;
        }

        // finally, add the faces as indices
        for (const Face& face : faces)
        {
            m_indices.push_back(face.v0);
            m_indices.push_back(face.v1);
            m_indices.push_back(face.v2);
        }
    }

    unsigned int IcoSphereMesh::get_mid_point(unsigned int v0, unsigned int v1)
    {
        // generate a key that is the same for edge v0 --- v1 and v1 --- v0
        bool first_smaller = v0 < v1;
        long smaller_index = first_smaller ? v0 : v1;
        long greater_index = first_smaller ? v1 : v0;
        long key = (smaller_index << 32) + greater_index;

        // return the cached point if it exists
        auto cached_point = m_mid_point_cache.find(key);
        if (cached_point != m_mid_point_cache.end())
        {
            return cached_point->second;
        }

        // mid between v0 and v1
        float mid_x = (m_vertices[v0 * 3 + 0] + m_vertices[v1 * 3 + 0]) * 0.5f;
        float mid_y = (m_vertices[v0 * 3 + 1] + m_vertices[v1 * 3 + 1]) * 0.5f;
        float mid_z = (m_vertices[v0 * 3 + 2] + m_vertices[v1 * 3 + 2]) * 0.5f;

        // add vertex to cache
        unsigned int mid_index = add_vertex(mid_x, mid_y, mid_z);
        m_mid_point_cache.emplace(key, mid_index);
        return mid_index;
    }

    unsigned int IcoSphereMesh::add_vertex(float x, float y, float z)
    {
        float length = std::sqrt(x * x + y * y + z * z);
        float nx = x / length;
        float ny = y / length;
        float nz = z / length;

        // add vertex position
        m_vertices.push_back(nx * 0.5f);
        m_vertices.push_back(ny * 0.5f);
        m_vertices.push_back(nz * 0.5f);

        // since it is a unit sphere, the normal is equal to the vertex position
        m_normals.push_back(nx);
        m_normals.push_back(ny);
        m_normals.push_back(nz);

        return m_index++;
    }

    const std::vector<float>& IcoSphereMesh::vertices()
    {
        return m_vertices;
    }

    const std::vector<unsigned int>& IcoSphereMesh::indices()
    {
        return m_indices;
    }

    const std::vector<float>& IcoSphereMesh::normals()
    {
        return m_normals;
    }
}