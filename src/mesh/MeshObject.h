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
        explicit MeshObject(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f>* mesh);

        ~MeshObject() = default;

        OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> *m_mesh;

        void load_from_file(std::string file_path);
        void write_to_file(const std::string& file_path) const;
        void set_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> *mesh);
        void set_highlight(OpenVolumeMesh::VertexHandle v_h, bool bl, float r, float g, float b, float a);
        void remove_highlights();
        void update_vertex_buffer();
        unsigned int to_faceID(unsigned int value);

        void init_vertices();
        void init_edges();
        void init_faces();
        //void init_cells();
        void init_face_normals();
        void init_vertex_normals();
        unsigned int to_faceID(int value);
        unsigned int to_edgeID(int value);

        std::vector<float>& vertices(){ return m_vertices;};
        std::vector<unsigned int>& edges(){ return m_edges;};
        std::vector<unsigned int>& faces(){ return m_faces;};
        std::vector<float>& vertex_normals(){ return m_vertex_normals;};
        std::vector<float>& face_normals(){ return m_face_normals;};
        std::tuple<int, int>& selection_offset(){ return m_selection_offset;};
        void set_selection_offset(int start);

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
        [[nodiscard]] int get_num_visible_vertices() const;

        [[nodiscard]] VertexArrayObject* get_cylinder_vao() const;
        [[nodiscard]] int get_num_visible_edges() const;

    private:
        void calculate_mesh_offset();
        int calculate_selection_size();

        std::vector<float> m_vertices;
        std::vector<unsigned int> m_edges;
        std::vector<unsigned int> m_faces;
        std::vector<float> m_vert_colors;
        std::vector<float> m_face_colors;
        std::vector<float> m_vertex_normals;
        std::vector<float> m_face_normals;
        std::vector<unsigned int> m_face_ids;
        std::vector<unsigned int> m_edge_ids;

        std::tuple<int, int> m_selection_offset;
        glm::vec3 m_mesh_offset_from_center;
        std::vector<std::tuple<OpenVolumeMesh::VertexHandle, float, float, float, float>> m_vertex_colors;
        VertexArrayObject* m_vertexArrayObject = nullptr;
        VertexArrayObject* m_sphere_vao = nullptr;
        VertexArrayObject* m_cylinder_vao = nullptr;

        bool m_should_update;

    };
}

#endif //VOLUMESHOS_MESH_OBJECT_H
