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
        MeshData() : m_color(0.35f, 0.35f, 0.35f, 0.7f), m_visible(true), rendering_mode("mesh_phong")
        {
            m_peel_level = 0;
            m_slice_level = 0;
            m_cell_size = 1;
        }

        [[nodiscard]] glm::mat4 get_transform() const
        {
            glm::mat4 pos = glm::translate(position);
            glm::mat4 scl = glm::scale(scale);
            return pos * scl;
        }

        int m_peel_level;
        bool m_slice_locked = false;
        float m_slice_level;
        float m_cell_size;
        bool m_visible;
        int selection_offset = 0;

        std::string rendering_mode;

        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f);

        Color m_color;

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

        explicit MeshObject(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d> *mesh);

        ~MeshObject();

        OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d> *m_mesh;

        std::unordered_set<int> &get_all_selected_faces()
        { return m_selected_faces; }

        std::unordered_set<int> &get_all_selected_vertices()
        { return m_selected_vertices; }

        std::unordered_set<int> &get_all_selected_edges()
        { return m_selected_edges; }

        std::unordered_set<int> &get_all_selected_cells()
        { return m_selected_cells; }

        /**
         * Adds a shape on selected element (vertex, edge, face)
         * @param id ID to access element data
         * @param type declares type of element
         */
        void select_element(int id, int type);

        /**
         * Removes a shape on selected element (vertex, edge, face)
         * @param id ID to access element data
         * @param type declares type of element
         */
        void unselect_element(int id, int type);

        /**
         * Removes all shapes added by selection
         */
        void unselect_all();

        /**
         * Checks if a specific element is selected
         * @param id ID to access element data
         * @param type declares type of element
         * @return
         */
        bool is_element_selected(int id, int type);

        void set_data(MeshData data)
        { m_data = std::move(data); }

        /**
         * Uses OVM FileManager to load Mesh from file
         * @param file_path path to file
         */
        void load_from_file(std::string file_path);

        /**
         * Uses OVM FileManager to save Mesh to file
         * @param file_path path to file
         */
        void write_to_file(const std::string &file_path) const;

        void set_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d> *mesh);

        /**
         * Calculate the amount of needed selection ids
         * @param start id where ids start
         */
        void set_selection_offset(int start);

        /**
         * updates the vertex_buffer
         */
        void update_vertex_buffer();

        /**
         * converts selection id of vertices to OVM id
         * @param value id value
         * @return
         */
        int to_vertexID(int value);

        /**
         * converts selection id of edges to OVM id
         * @param value id value
         * @return
         */
        int to_edgeID(int value);

        /**
         * converts selection id of faces to OVM id
         * @param value id value
         * @return
         */
        int to_faceID(int value);


        int get_max_peel_depth() const;

        std::tuple<int, int> &selection_offset()
        { return m_selection_offset; };

        glm::vec3 &get_mesh_offset();

        glm::vec3 &get_min()
        { return m_min; };

        glm::vec3 &get_max()
        { return m_max; };

        /**
         * Calculates bounding box of transformed vertices. Used for slicing into camera direction.
         * @param transform Transformation matrix
         * @return bounding box in mesh coorinates
         */
        std::pair<glm::vec3, glm::vec3> &get_transformed_bb(const glm::mat4 &transform);

        /**
         * Calculates the direction the camera points to
         * @param transform Transformation matrix
         * @param view_dir camera direction
         * @return direction vector
         */
        glm::vec3 &get_slice_dir(const glm::mat4 &transform, const glm::vec3 &view_dir);

        MeshData &get_data()
        { return m_data; }

        [[nodiscard]] VertexArrayObject *get_vao() const;



        /**
         * This is here for rendering the per vertex sphere picking. It must be in this class, because anywhere else,
         * we would have to update the vertex array with the data every time we render.
         *
         * @return the instanced sphere vao for this mesh
         */

        [[nodiscard]] int get_num_visible_vertices() const;
        [[nodiscard]] int get_num_visible_edges() const;

        [[nodiscard]] VertexArrayObject *get_cylinder_vao() const;
        [[nodiscard]] VertexArrayObject *get_sphere_vao() const;



    private:
        /**
         * Gets the center of the mesh (e.g for rotation) by calculating the bounding_box
         */
        void calculate_mesh_offset();

        /**
         * Calculates the depth of vertices and cells
         */
        void calculate_peel_depth();

        /**
         * calculates the amount of needed ids
         * @return
         */
        [[nodiscard]] int calculate_selection_size() const;

        const int key_multiplier = 1000000;

        int m_max_peel_depth = 0;

        bool m_just_locked;

        bool m_should_update;

        std::vector<float> m_vert_colors;

        std::vector<float> m_face_colors;

        std::unordered_set<int> m_selected_faces;

        std::unordered_set<int> m_selected_vertices;

        std::unordered_set<int> m_selected_edges;

        std::unordered_set<int> m_selected_cells;

        std::map<int, int> m_created_shapes;

        std::map<OpenVolumeMesh::VertexHandle, Highlight> highlight_map;

        std::tuple<int, int> m_selection_offset;

        std::pair<glm::vec3, glm::vec3> m_transformed_bb;

        glm::vec3 m_mesh_offset_from_center;

        glm::vec3 m_min;

        glm::vec3 m_max;

        glm::vec3 m_slice_dir;

        MeshVertexBuffer *m_mvb = nullptr;

        MeshData m_data;

    };
}
