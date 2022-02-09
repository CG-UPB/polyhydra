#pragma once

#include <OpenVolumeMesh/Geometry/VectorT.hh>
#include <OpenVolumeMesh/Core/GeometryKernel.hh>
#include <utility>
#include <vector>
#include <map>
#include <unordered_set>
#include "../rendering/gl/VertexArrayObject.h"
#include "glm/gtx/transform.hpp"
#include "MeshVertexBuffer.h"

namespace vOS
{

    struct Color
    {
        Color(float _r, float _g, float _b) : r(_r), g(_g), b(_b), a(1)
        {}

        Color(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a)
        {}

        [[nodiscard]] glm::vec3 get_rgb() const
        { return {r, g, b}; }

        [[nodiscard]] glm::vec4 get_rgba() const
        { return {r, g, b, a}; }

        float r;
        float g;
        float b;
        float a;
    };

    struct MeshData
    {
        MeshData() : m_color(0.35f, 0.35f, 0.35f, 1), m_visible(true), rendering_mode("mesh_phong")
        {
            m_peel_level = 0;
            m_slice_level =0;
            m_cell_size = 1;
        }

        [[nodiscard]] glm::mat4 get_transform() const
        {
            glm::mat4 pos = glm::translate(position);
            glm::mat4 scl = glm::scale(scale);
            return pos * scl;
        }

        Color m_color;
        bool m_visible;
        std::string rendering_mode;

        int m_peel_level;
        bool m_slice_locked = false;
        float m_slice_level;
        float m_cell_size;

        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f);
        int selection_offset = 0;
    };

    struct Highlight
    {
        Highlight(Color c, OpenVolumeMesh::VertexHandle vh) : color(c), v_h(vh)
        {}

        Color color;
        OpenVolumeMesh::VertexHandle v_h;
    };

    class MeshObject
    {
    public:

        MeshObject();

        explicit MeshObject(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f>* mesh);

        ~MeshObject();

        OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f>* m_mesh;

        // Selection Functionality
        std::unordered_set<int>& get_all_selected_faces()
        { return m_selected_faces; }

        std::unordered_set<int>& get_all_selected_vertices()
        { return m_selected_vertices; }

        std::unordered_set<int>& get_all_selected_edges()
        { return m_selected_edges; }

        std::unordered_set<int>& get_all_selected_cells()
        { return m_selected_cells; }

        void select_element(int id, int type);

        void unselect_element(int id, int type);

        void unselect_all();

        bool is_element_selected(int id, int type);

        MeshData& get_data()
        { return m_data; }

        void set_data(MeshData data)
        { m_data = std::move(data); }

        void load_from_file(std::string file_path);

        void write_to_file(const std::string& file_path) const;

        void set_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f>* mesh);

        void add_highlight(Highlight tuple);

        void remove_highlight(OpenVolumeMesh::VertexHandle vh);

        void remove_highlights();

        void update_vertex_buffer();

        int to_vertexID(int value);

        int to_edgeID(int value);

        int to_faceID(int value);

        std::tuple<int, int>& selection_offset()
        { return m_selection_offset; };

        void set_selection_offset(int start);

        std::map<OpenVolumeMesh::VertexHandle, Highlight>& get_highlights();

        glm::vec3& get_mesh_offset();

        [[nodiscard]] VertexArrayObject* get_vao() const;

        glm::vec3& get_min()
        { return m_min; };

        glm::vec3& get_max()
        { return m_max; };

        std::pair<glm::vec3, glm::vec3>& get_transformed_bb(const glm::mat4& transform);

        glm::vec3& get_slice_dir(const glm::mat4& transform, const glm::vec3& view_dir);

        int get_max_peel_depth() const;

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

        [[nodiscard]] int calculate_selection_size() const;

        void calculate_peel_depth();

        const int key_multiplier = 1000000;

        std::vector<float> m_vert_colors;
        std::vector<float> m_face_colors;
        std::unordered_set<int> m_selected_faces;
        std::unordered_set<int> m_selected_vertices;
        std::unordered_set<int> m_selected_edges;
        std::unordered_set<int> m_selected_cells;
        std::map<int, int> m_created_shapes;

        std::map<OpenVolumeMesh::VertexHandle, Highlight> highlight_map;

        std::tuple<int, int> m_selection_offset;
        glm::vec3 m_mesh_offset_from_center;

        MeshVertexBuffer* m_mvb = nullptr;

        MeshData m_data;

        bool m_should_update;
        glm::vec3 m_min;
        glm::vec3 m_max;

        int m_max_peel_depth = 0;
        std::pair<glm::vec3, glm::vec3> m_transformed_bb;
        glm::vec3 m_slice_dir;
        bool m_just_locked;
    };
}
