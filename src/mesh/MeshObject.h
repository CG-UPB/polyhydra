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
#include "nlohmann/json.hpp"
#include "../util/VecUtil.h"

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
        MeshData() : m_color(0.76f, 0.76f, 0.76f, 1), m_selection_color(0.76f, 0.76f, 0.76f, 0), m_visible(true), m_rendering_mode("mesh_phong")
        {
            m_peel_level = 0;
            m_slice_level = 0;
            m_cell_size = 1;

            m_ambient_strength = 1.0f;
            m_diffuse_strength = 1.0f;
            m_specular_strength = 0.3f;
            m_specular_exponent = 8.0f;
        }

         [[nodiscard]] nlohmann::json to_json()
         {
            nlohmann::json j;

            // Phong Data
            j["phong_spec_strength"] = m_specular_strength;
            j["phon_spec_exponent"] = m_specular_exponent;
            j["phong_ambient_strength"] = m_ambient_strength;
            j["phong_diffuse_strength"] = m_diffuse_strength;

            // Toolbox Data
            j["tool_slice_level"] = m_slice_level;
            j["tool_slice_locked"] = m_slice_locked;
            j["tool_peel_level"] = m_peel_level;
            j["tool_cell_size"] = m_cell_size;

             // Rendering Data
             j["rendering_default_color"] = {m_color.r, m_color.g, m_color.b, m_color.a};
             j["rendering_visible"] = m_visible;
             j["rendering_mode"] = m_rendering_mode;
             j["rendering_selection_color"] =  {m_selection_color.r, m_selection_color.g, m_selection_color.b, m_selection_color.a};

             // Transform Data
             j["transform_position"] = {m_position.x, m_position.y, m_position.z};
             j["transform_scale"] = {m_scale.x, m_scale.y, m_scale.z};

             // Roundings
             j["roundings activated"] = m_rounding_activated;
             j["rounding size"] = m_rounding_size;

            return j;
         }

         void load_from_json(nlohmann::json j)
         {
             // Phong Data
             m_specular_strength = j["phong_spec_strength"];
             m_specular_exponent = j["phon_spec_exponent"];
             m_ambient_strength = j["phong_ambient_strength"];
             m_diffuse_strength = j["phong_diffuse_strength"];

             // Toolbox Data
             m_slice_level = j["tool_slice_level"];
             m_slice_locked = j["tool_slice_locked"];
             m_peel_level = j["tool_peel_level"];
             m_cell_size = j["tool_cell_size"];

             // Rendering Data
             auto color_vec = j["rendering_default_color"];
             m_color = Color(color_vec[0], color_vec[1], color_vec[2], color_vec[3]);
             m_visible = j["rendering_visible"];
             m_rendering_mode = j["rendering_mode"];
             color_vec = j["rendering_selection_color"];
             m_selection_color =  Color(color_vec[0], color_vec[1], color_vec[2], color_vec[3]);


             // Transform Data
             auto pos_vec = j["transform_position"];
             m_position = glm::vec3(pos_vec[0], pos_vec[1], pos_vec[2]);
             auto scale_vec = j["transform_scale"];
             m_scale = glm::vec3(scale_vec[0], scale_vec[1], scale_vec[2]);

             // Roundings
             m_rounding_activated = j["roundings activated"];
             m_rounding_size = j["rounding size"];
         }

        [[nodiscard]] glm::mat4 get_transform() const
        {
            glm::mat4 pos = glm::translate(m_position);
            glm::mat4 scl = glm::scale(m_scale * scale_normalization);
            return pos * scl * glm::translate(-m_offset);
        }

        // Rendering Variables
        Color m_color;
        Color m_selection_color;
        bool m_visible;
        std::string m_rendering_mode;

        float m_ambient_strength;
        float m_diffuse_strength;
        float m_specular_strength;
        float m_specular_exponent;

        // Toolbox Variables
        float m_peel_level;
        bool m_slice_locked = false;
        float m_slice_level;
        float m_cell_size;
        int m_selection_offset = 0;
        float scale_normalization = 1.0f;

        std::string rendering_mode;

        // Transform Variables
        glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 m_offset = glm::vec3(0.0f, 0.0f, 0.0f);

        //Rounding Variables
        bool m_rounding_activated = false;
        float m_rounding_size = 0.3f;
    };

    class MeshObject
    {
    public:

        MeshObject();

        explicit MeshObject(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d> *mesh, std::string name);

        ~MeshObject();

        OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d> *m_mesh;

        // Selection Functionality
        std::unordered_set<int>& get_all_selected_faces()
        { return m_selected_faces; }

        std::unordered_set<int>& get_all_selected_vertices()
        { return m_selected_vertices; }

        std::unordered_set<int>& get_all_selected_edges()
        { return m_selected_edges; }

        std::unordered_set<int>& get_all_selected_cells()
        { return m_selected_cells; }

        MeshVertexBuffer* get_mesh_vertex_buffer(){return m_mvb;}

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
        void load_from_file(const std::string& file_path);

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

        void set_face_color(int ovm_id, Color color);

        /**
         * updates the vertex_buffer
         */
        void update_vertex_buffer();

        /**
         * converts selection id of vertices to OVM id
         * @param value id value
         * @return
         */
        int to_vertex_id(int value);

        /**
         * converts selection id of edges to OVM id
         * @param value id value
         * @return
         */
        int to_edge_id(int value);

        /**
         * converts selection id of faces to OVM id
         * @param value id value
         * @return
         */
        int to_halfface_id(int value);


        int get_max_peel_depth() const;

        std::tuple<int, int> &selection_offset()
        { return m_selection_offset; };

        glm::vec3& get_mesh_offset();

        [[nodiscard]] VertexArrayObject* get_vao() const;

        glm::vec3 get_min();

        glm::vec3 get_max();

        /**
         * Calculates bounding box of transformed vertices. Used for slicing into camera direction.
         * @param transform Transformation matrix
         * @return bounding box in mesh coorinates
         */
        std::pair<glm::vec3, glm::vec3> &get_transformed_bb(const glm::mat4 &transform);

        /**
         * Calculates the direction the camera points to
         * @param view_transform Transformation matrix
         * @param view_dir camera direction
         * @return direction vector
         */
        glm::vec3 &get_slice_dir(const glm::mat4 &view_transform, const glm::vec3 &view_dir);

        MeshData &get_data()
        { return m_data; }



        void set_mesh_name(std::string str){mesh_name = str;}
        std::string get_mesh_name(){return mesh_name;}

        /**
         * This is here for rendering the per vertex sphere picking. It must be in this class, because anywhere else,
         * we would have to update the vertex array with the data every time we render.
         *
         * @return the instanced sphere vao for this mesh
         */

        [[nodiscard]] MeshVertexBuffer* get_mvb() const;

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

        std::string mesh_name = "default";

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


        std::tuple<int, int> m_selection_offset;

        std::pair<glm::vec3, glm::vec3> m_transformed_bb;

        glm::vec3 m_mesh_offset_from_center;

        glm::vec3 m_slice_dir;

        MeshVertexBuffer *m_mvb = nullptr;

        MeshData m_data;

    };
}
