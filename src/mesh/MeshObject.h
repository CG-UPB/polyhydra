#include <OpenVolumeMesh/Geometry/VectorT.hh>
#include <OpenVolumeMesh/Core/GeometryKernel.hh>
#include <vector>
#include <map>
#include "../rendering/VertexArrayObject.h"
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

        ~MeshObject() = default;

        bool m_is_rendering;

        OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> *m_mesh;

        void load_from_file(std::string file_path);
        void write_to_file(std::string file_path);
        void set_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> *mesh);
        void add_highlight(Highlight tuple);
        void remove_highlight(OpenVolumeMesh::VertexHandle vh);
        void remove_highlights();
        void initialize_face_normals();
        void initialize_vertex_normals();
        void update_vertex_buffer();

        std::vector<float> vertices();
        std::vector<unsigned int> edges();
        std::vector<unsigned int> faces();
        std::vector<float> vertex_normals();
        std::vector<float> face_normals();
        std::map<OpenVolumeMesh::VertexHandle, Highlight> get_highlights();


        glm::vec3 &get_mesh_offset();
        [[nodiscard]] VertexArrayObject* get_vao() const;

    private:
        void calculate_mesh_offset();

        std::vector<float> m_vertices;
        std::vector<unsigned int> m_indices;
        std::vector<float> m_vertex_normals;
        std::vector<float> m_face_normals;

        std::map<OpenVolumeMesh::VertexHandle, Highlight> highlight_map;

        VertexArrayObject *m_vertexArrayObject = nullptr;

        glm::vec3 m_mesh_offset_from_center;

        bool m_should_update;
    };


}

#endif //VOLUMESHOS_MESH_OBJECT_H
