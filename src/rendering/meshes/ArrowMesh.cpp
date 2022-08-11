
#include "ArrowMesh.h"

#include "ClosedCylinderMesh.h"
#include "ConeMesh.h"

namespace volumeshOS::Internal
{
    ArrowMesh::ArrowMesh(int samples, float base_width_percentage, float tip_height_percentage)
    {
        float tip_start_height = 1.0f - tip_height_percentage;

        ConeMesh cone{samples, 0.5f, tip_height_percentage};
        ClosedCylinderMesh cylinder{samples, 0.5f * base_width_percentage, tip_start_height};

        std::vector<float> cylinder_vertices = cylinder.vertices();
        std::vector<uint32_t> cylinder_indices = cylinder.indices();
        std::vector<float> cylinder_normals = cylinder.normals();

        for (int i = 0; i < cone.vertices.size() / 3; i++)
        {
            // adjust height of cone (y position)
            cone.vertices[i * 3 + 1] += tip_start_height;
        }
        int index_offset = (int) cone.vertices.size() / 3;

        for (uint32_t& cylinder_index : cylinder_indices)
        {
            // apply offset to indices of the cylinder
            cylinder_index += index_offset;
        }

        for (int i = 0; i < cylinder_vertices.size() / 3; i++)
        {
            // adjust height of cylinder
            cylinder_vertices[i * 3 + 1] += tip_start_height * 0.5f;
        }

        vertices.insert(vertices.end(), cone.vertices.begin(), cone.vertices.end());
        vertices.insert(vertices.end(), cylinder_vertices.begin(), cylinder_vertices.end());

        indices.insert(indices.end(), cone.indices.begin(), cone.indices.end());
        indices.insert(indices.end(), cylinder_indices.begin(), cylinder_indices.end());

        normals.insert(normals.end(), cone.normals.begin(), cone.normals.end());
        normals.insert(normals.end(), cylinder_normals.begin(), cylinder_normals.end());
    }
}