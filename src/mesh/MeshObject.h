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

        OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> *m_mesh;

        void load_from_file(std::string file_path);
        void write_to_file(std::string file_path);
        void set_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> *mesh);
        void set_highlight(OpenVolumeMesh::VertexHandle v_h, bool bl, float r, float g, float b, float a);
        void remove_highlights();
        void initialize_face_normals();
        void initialize_vertex_normals();
        void update_vertex_buffer();
        unsigned int to_faceID(int value);

        std::vector<float> vertices();
        std::vector<unsigned int> edges();
        std::vector<unsigned int> faces();
        std::vector<float> vertex_normals();
        std::vector<float> face_normals();
        std::vector<std::tuple<OpenVolumeMesh::VertexHandle, float, float, float, float>> get_highlights();


        glm::vec3 &get_mesh_offset();
        [[nodiscard]] VertexArrayObject* get_vao() const;

        /**
         * This is here for rendering the per vertex sphere picking. It must be in this class, because anywhere else,
         * we would have to update the vertex array with the data every time we render.
         *
         * @return the instanced sphere vao for this mesh
         */
        [[nodiscard]] VertexArrayObject* get_sphere_vao() const;

        [[nodiscard]] int get_num_vertices() const;

    private:
        void calculate_mesh_offset();

        std::vector<float> m_vertices;
        std::vector<unsigned int> m_indices;
        std::vector<float> m_vert_colors;
        std::vector<float> m_face_colors;
        std::vector<float> m_vertex_normals;
        std::vector<float> m_face_normals;
        std::vector<unsigned int> m_face_ids;
        std::vector<std::tuple<OpenVolumeMesh::VertexHandle, float, float, float, float>> m_vertex_colors;

        VertexArrayObject* m_vertexArrayObject = nullptr;
        VertexArrayObject* m_sphere_vao = nullptr;

        glm::vec3 m_mesh_offset_from_center;

        bool m_should_update;
    };


}

#endif //VOLUMESHOS_MESH_OBJECT_H
