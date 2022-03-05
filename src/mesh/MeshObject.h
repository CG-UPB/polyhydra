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

namespace vOS
{

    struct Color
    {
        Color(float _r, float _g, float _b) : r(_r), g(_g), b(_b), a(1)
        {}

        Color(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a)
        {}

        [[nodiscard]] glm::vec3 get() const
        { return {r, g, b}; }

        [[nodiscard]] glm::vec4 get_a() const
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
            m_slice_level =0;
            m_cell_size = 1;

            m_ambient_strength = 0.3;
            m_diffuse_strength = 1;
            m_specular_strength = 0.5;
            m_specular_exponent = 8;
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
             j["transform_position_offset"] = m_selection_offset;
             j["transform_scale"] = {m_scale.x, m_scale.y, m_scale.z};
             j["transform_offset"] = {m_offset.x, m_offset.y, m_offset.z};

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
             m_selection_offset = j["transform_position_offset"];
             auto pos_vec = j["transform_position"];
             m_position = glm::vec3(pos_vec[0], pos_vec[1], pos_vec[2]);
             auto scale_vec = j["transform_scale"];
             m_scale = glm::vec3(scale_vec[0], scale_vec[1], scale_vec[2]);
             auto off_vec = j["transform_offset"];
             m_offset = glm::vec3(off_vec[0], off_vec[1], off_vec[2]);
         }

        [[nodiscard]] glm::mat4 get_transform() const
        {
            glm::mat4 pos = glm::translate(m_position);
            glm::mat4 scl = glm::scale(m_scale * scale_normalization);
            return pos * scl;
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
        int m_peel_level;
        bool m_slice_locked = false;
        float m_slice_level;
        float m_cell_size;

        // Transform Variables
        glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 m_offset = glm::vec3(0.0f, 0.0f, 0.0f);
        int m_selection_offset = 0;
        float scale_normalization = 1.0f;
    };

    class MeshObject
    {
    public:

        MeshObject();

        explicit MeshObject(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f>* mesh, std::string name);

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

        MeshVertexBuffer* get_mesh_vertex_buffer(){return m_mvb;}

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

        void update_vertex_buffer();

        int to_vertexID(int value);

        int to_edgeID(int value);

        int to_faceID(int value);

        std::tuple<int, int>& selection_offset()
        { return m_selection_offset; };

        void set_selection_offset(int start);

        glm::vec3& get_mesh_offset();

        [[nodiscard]] VertexArrayObject* get_vao() const;

        glm::vec3& get_min()
        { return m_min; };

        glm::vec3& get_max()
        { return m_max; };

        std::pair<glm::vec3, glm::vec3>& get_transformed_bb(const glm::mat4& transform);

        glm::vec3& get_slice_dir(const glm::mat4& transform, const glm::vec3& view_dir);

        int get_max_peel_depth() const;

        void set_mesh_name(std::string str){mesh_name = str;}
        std::string get_mesh_name(){return mesh_name;}

        /**
         * This is here for rendering the per vertex sphere picking. It must be in this class, because anywhere else,
         * we would have to update the vertex array with the data every time we render.
         *
         * @return the instanced sphere vao for this mesh
         */
        [[nodiscard]] VertexArrayObject* get_sphere_vao() const;

        [[nodiscard]] MeshVertexBuffer* get_mvb() const;

        [[nodiscard]] int get_num_visible_vertices() const;

        [[nodiscard]] VertexArrayObject* get_cylinder_vao() const;

        [[nodiscard]] int get_num_visible_edges() const;

    private:
        void calculate_mesh_offset();

        [[nodiscard]] int calculate_selection_size() const;

        void calculate_peel_depth();

        const int key_multiplier = 1000000;

        std::string mesh_name = "default";

        std::vector<float> m_vert_colors;
        std::vector<float> m_face_colors;
        std::unordered_set<int> m_selected_faces;
        std::unordered_set<int> m_selected_vertices;
        std::unordered_set<int> m_selected_edges;
        std::unordered_set<int> m_selected_cells;
        std::map<int, int> m_created_shapes;


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
