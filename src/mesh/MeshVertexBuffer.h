#pragma once

#include <OpenVolumeMesh/Core/GeometryKernel.hh>
#include "../rendering/gl/VertexArrayObject.h"
#include "glm/gtx/transform.hpp"
#include <map>

namespace vOS
{
    typedef OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> Mesh;
    typedef OpenVolumeMesh::CellHandle Cell;

    struct BufferSpecification
    {
        int peel_depth = 0;
        int slice_depth = 0;
    };

    struct VertexData
    {
        OpenVolumeMesh::VertexHandle ovm_handle;
        glm::vec3 position{};
        glm::vec3 normal{};
        glm::vec3 color{};
    };

    struct FaceData
    {
        bool is_boundary = false;
        std::vector<VertexData> vertices;
        std::vector<unsigned int> indices;
        std::vector<unsigned int> face_ids;
    };

    class MeshVertexBuffer
    {
    public:

        MeshVertexBuffer(){};

        explicit MeshVertexBuffer(Mesh* mesh, BufferSpecification spec);

        ~MeshVertexBuffer();

        int to_vertexID(int value);

        int to_edgeID(int value);

        int to_faceID(int value);

        std::vector<float>& get_original_vertices();

        glm::vec3 get_center(const std::vector<glm::vec3>& vertices);

        VertexArrayObject* get_vao();

        void set_face_color(int ovm_id, float r, float g, float b, float a);

        VertexArrayObject* get_sphere_vao();

        VertexArrayObject* get_cylinder_vao();

        [[nodiscard]] int get_num_selection_vertices() const;

        [[nodiscard]] int get_num_selection_edges() const;

        void update_digging_buffer(int id, float newValue);

    private:

        void generate_buffer(Mesh& mesh);

        void add_cell(Mesh& mesh, Cell cell);

        void add_face_indices(Mesh& mesh, FaceData& face);

        void add_from_to_vertex(Mesh& mesh, const OpenVolumeMesh::VertexHandle& from, const OpenVolumeMesh::VertexHandle& to);



        std::pair<glm::vec3,glm::vec3> get_bounding_box(const std::vector<glm::vec3>& vertices);

        std::vector<float> get_vertices(Mesh& mesh);

        BufferSpecification m_spec;

        /**
         * Set to true, if some update has been made to the vao buffers (like face color)
         * To reduce overhead we do not update the vao immediatly, but only before it has been requested by some other class
         */
        bool m_update_vao = false;
        int m_face_amount = 0;
        int m_cell_start_face_index = 0;

        std::vector<float> m_original_vertices;

        VertexArrayObject* m_vao = nullptr;
        VertexArrayObject* m_sphere_vao = nullptr;
        VertexArrayObject* m_cylinder_vao = nullptr;

        // ovm ids, in the order that we render them
        std::vector<int> m_vertex_ids;
        std::vector<int> m_edge_ids;
        std::vector<int> m_face_ids;

        // vertex attributes
        std::vector<float> m_positions;
        std::vector<float> m_normals;
        std::vector<float> m_cell_centers;
        std::vector<float> m_is_face_boundary;
        std::vector<float> m_colors;
        std::vector<float> m_sphere_cell_centers;
        std::vector<float> m_cylinder_cell_centers;
        std::vector<float> m_sphere_peel_depths;
        std::vector<float> m_cylinder_peel_depths;
        std::vector<float> m_peel_depths;

        std::vector<float> m_sphere_is_digged;
        std::vector<float> m_cylinder_is_digged;
        std::vector<float> m_is_digged;

        std::vector<unsigned int> m_indices;
        std::vector<float> m_from_vertices;
        std::vector<float> m_to_vertices;
        std::vector<float> m_selection_vertices;

        /**
         * Maps OVM Ids to face buffer locations
         */
        std::map<int,int> m_ovm_to_gl_face_indizes;
        std::vector<int> m_face_offset_array;
        std::vector<int> m_face_vertex_count;
        int m_total_vertex_count = 0;

        std::map<int,int> m_start_of_cell_vertices;

        std::map<int,int> m_size_of_cell_vertices;


        int m_num_vertices = 0;
    };
}