#include <OpenVolumeMesh/Geometry/VectorT.hh>
#include <OpenVolumeMesh/Core/GeometryKernel.hh>
#include <vector>
#include <map>
#include "../rendering/gl/VertexArrayObject.h"
#include "glm/gtx/transform.hpp"

#ifndef VOLUMESHOS_MESH_OBJECT_H
#define VOLUMESHOS_MESH_OBJECT_H

namespace vOS
{
    struct Color
    {
        Color(float _r, float _g, float _b) : r(_r), g(_g), b(_b), a(1){}
        Color(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a){}
        float r;
        float g;
        float b;
        float a;
    };

    struct Highlight
    {
        Highlight(Color c, OpenVolumeMesh::VertexHandle vh) : color(c), v_h(vh){}
        Color color;
        OpenVolumeMesh::VertexHandle v_h;
    };

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
        void add_highlight(Highlight tuple);
        void remove_highlight(OpenVolumeMesh::VertexHandle vh);
        void remove_highlights();
        void update_vertex_buffer();

        void init_vertices();
        void init_edges();
        void init_faces();
        //void init_cells();
        void init_face_normals();
        void init_vertex_normals();
        unsigned int to_faceID(unsigned int value);
        unsigned int to_edgeID(unsigned int value);

        std::vector<float>& vertices(){ return m_vertices;};
        std::vector<unsigned int>& edges(){ return m_edges;};
        std::vector<unsigned int>& faces(){ return m_faces;};
        std::vector<float>& vertex_normals(){ return m_vertex_normals;};
        std::vector<float>& face_normals(){ return m_face_normals;};
        std::tuple<int, int> selection_offset(){ return m_selection_offset;};
        void set_selection_offset(int start);
        std::map<OpenVolumeMesh::VertexHandle, Highlight> get_highlights();

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
        int calculate_selection_size() const;

        std::vector<float> m_vertices;
        std::vector<unsigned int> m_edges;
        std::vector<unsigned int> m_faces;
        std::vector<float> m_vert_colors;
        std::vector<float> m_face_colors;
        std::vector<float> m_vertex_normals;
        std::vector<float> m_face_normals;

        std::map<OpenVolumeMesh::VertexHandle, Highlight> highlight_map;
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
