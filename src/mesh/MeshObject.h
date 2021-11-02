#include <OpenVolumeMesh/Geometry/VectorT.hh>
#include <OpenVolumeMesh/Core/GeometryKernel.hh>
#include <vector>
#include "../rendering/VertexArrayObject.h"
#include "glm/gtx/transform.hpp"

#ifndef VOLUMESHOS_MESH_OBJECT_H
#define VOLUMESHOS_MESH_OBJECT_H

namespace vOS
{
    class MeshObject
    {
    public:

        MeshObject();
        ~MeshObject() = default;

        bool m_is_rendering;

        bool m_should_update;
        OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> *m_mesh;

        void load_from_file(std::string file_path);
        void write_to_file(std::string file_path);
        void set_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> *mesh);

        void draw();

        std::vector<float> vertices();
        std::vector<unsigned int> edges();
        std::vector<unsigned int> faces();

        glm::vec3& get_mesh_offset();

        void set_highlight(OpenVolumeMesh::VertexHandle v_h, bool bl, float r, float g, float b, float a);
        std::vector<std::tuple<OpenVolumeMesh::VertexHandle, float, float, float, float>> get_highlights();
        void remove_highlights();

    private:

        void calculate_mesh_offset();
        void update_vertex_buffer();

        std::vector<float> m_vertices;
        std::vector<unsigned int> m_indices;
        std::vector<std::tuple<OpenVolumeMesh::VertexHandle, float, float, float, float>> m_vertex_colors;

        VertexArrayObject* m_vertexArrayObject = nullptr;
        glm::vec3 m_mesh_offset_from_center;
    };





}

#endif //VOLUMESHOS_MESH_OBJECT_H
