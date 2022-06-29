#pragma once

#include "vospch.h"

#include "../rendering/gl/VertexArrayObject.h"
#include "MeshVertexBuffer.h"
#include "nlohmann/json.hpp"
#include "../util/VecUtil.h"

namespace volumeshOS::Internal
{

    struct MeshData
    {
        nlohmann::json to_json()
        {
            nlohmann::json j;

            // Phong Data
            j["phong_spec_strength"] = specular_strength;
            j["phong_spec_exponent"] = specular_exponent;
            j["phong_ambient_strength"] = ambient_strength;
            j["phong_diffuse_strength"] = diffuse_strength;

            // Toolbox Data
            j["tool_slice_level"] = slice_level;
            j["tool_slice_locked"] = slice_locked;
            j["tool_peel_level"] = peel_level;
            j["tool_cell_size"] = cell_size;

            // Rendering Data
            j["rendering_default_color"] = {color.r, color.g, color.b, color.a};
            j["rendering_visible"] = visible;
            j["rendering_selection_color"] = {selection_color.r, selection_color.g, selection_color.b,
                                              selection_color.a};

            // Transform Data
            j["transform_position"] = {position.x, position.y, position.z};
            j["transform_scale"] = {scale.x, scale.y, scale.z};

            // Roundings
            j["roundings activated"] = rounding_active;
            j["rounding size"] = rounding_size;

            return j;
        }

        void load_from_json(nlohmann::json j)
        {
            // Phong Data
            specular_strength = j["phong_spec_strength"];
            specular_exponent = j["phong_spec_exponent"];
            ambient_strength = j["phong_ambient_strength"];
            diffuse_strength = j["phong_diffuse_strength"];

            // Toolbox Data
            slice_level = j["tool_slice_level"];
            slice_locked = j["tool_slice_locked"];
            peel_level = j["tool_peel_level"];
            cell_size = j["tool_cell_size"];

            // Rendering Data
            auto color_vec = j["rendering_default_color"];
            color = Color(color_vec[0], color_vec[1], color_vec[2], color_vec[3]);
            visible = j["rendering_visible"];
            color_vec = j["rendering_selection_color"];
            selection_color = Color(color_vec[0], color_vec[1], color_vec[2], color_vec[3]);


            // Transform Data
            auto pos_vec = j["transform_position"];
            //position = glm::vec3(pos_vec[0], pos_vec[1], pos_vec[2]);
            auto scale_vec = j["transform_scale"];
            scale = glm::vec3(scale_vec[0], scale_vec[1], scale_vec[2]);

            // Roundings
            rounding_active = j["roundings activated"];
            rounding_size = j["rounding size"];
        }


        const glm::mat4& get_transform() const
        {
            return transformation;
        }

        void update_transform()
        {
            glm::mat4 rot(1.0f);
            rot = glm::translate(glm::mat4(1.0), (position )) * rotation * glm::translate(glm::mat4(1.0), -(position )) ;
            glm::mat4 scal(1.0f);
            scal = glm::translate(glm::mat4(1.0), (position )) * scaling * glm::translate(glm::mat4(1.0), -(position )) ;
            transformation =  scal * rot * translation;
        }

        glm::mat4 translation = glm::mat4(1.0f);
        glm::mat4 scaling = glm::mat4(1.0f);
        glm::mat4 rotation = glm::mat4(1.0f);
        glm::vec3 up_dir = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::mat4 transformation = glm::mat4(1.0f);

        // Rendering Variables
        Color color                 = Color{0.76f, 0.76f, 0.76f, 1.0f};
        Color selection_color       = Color{0.76f, 0.76f, 0.76f, 0.0f};
        float ambient_strength      = 1.0f;
        float diffuse_strength      = 1.0f;
        float specular_strength     = 0.3f;
        float specular_exponent     = 8.0f;

        // Toolbox Variables
        float peel_level            = 0.0f;
        float slice_level           = 0.0f;
        float cell_size             = 1.0f;
        bool slice_locked           = false;
        bool rounding_active        = false;
        float rounding_size         = 0.0f;

        // Transform Variables
        glm::vec3 position          = {0.0f, 0.0f, 0.0f};
        glm::vec3 scale             = {1.0f, 1.0f, 1.0f};
        glm::vec3 position_offset   = {0.0f, 0.0f, 0.0f};
        float scale_normalization   = 1.0f;

        // Other
        bool visible                = true;
        int selection_id_offset     = 0;
    };

    class MeshObject
    {
    public:

        explicit MeshObject(int id);

        // Selection Functionality
        std::unordered_set<int>& get_all_selected_faces()
        {
            return m_selected_faces;
        }

        std::unordered_set<int>& get_all_selected_vertices()
        {
            return m_selected_vertices;
        }

        std::unordered_set<int>& get_all_selected_edges()
        {
            return m_selected_edges;
        }

        std::unordered_set<int>& get_all_selected_cells()
        {
            return m_selected_cells;
        }

        /**
         * Adds a shape on selected element (vertex, edge, face)
         * @param id ID to access element data
         * @param type declares type of element
         */
        void select_element(int id, EntityType type);

        /**
         * Removes a shape on selected element (vertex, edge, face)
         * @param id ID to access element data
         * @param type declares type of element
         */
        void deselect_element(int id, EntityType type);

        /**
         * Removes all shapes added by selection
         */
        void deselect_all();

        /**
         * Checks if a specific element is selected
         * @param id ID to access element data
         * @param type declares type of element
         * @return
         */
        bool is_element_selected(int id, EntityType type);

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

        void set_cell_color(int ovm_id, Color color);

        void set_mesh_color(Color color);

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

        std::tuple<int, int>& selection_offset()
        {
            return m_selection_offset;
        };

        glm::vec3& get_mesh_offset();

        [[nodiscard]] std::shared_ptr<VertexArrayObject> get_vao() const;

        glm::vec3 get_min();

        glm::vec3 get_max();

        /**
         * Calculates bounding box of transformed vertices. Used for slicing into camera direction.
         * @param transform Transformation matrix
         * @return bounding box in mesh coorinates
         */
        std::pair<glm::vec3, glm::vec3> &get_world_bb(const glm::mat4 &transform);

        /**
         * Calculates the direction the camera points to
         * @param view_transform Transformation matrix
         * @param view_dir camera direction
         * @return direction vector
         */
        glm::vec3 &get_slice_dir(const glm::mat4 &view_transform, const glm::vec3 &view_dir);

        MeshData& get_data()
        {
            return m_data;
        }


        void set_mesh_name(const std::string &name)
        {
            m_mesh_name = name;
        }

        std::string get_mesh_name()
        {
            return m_mesh_name;
        }

        /**
         * This is here for rendering the per vertex sphere picking. It must be in this class, because anywhere else,
         * we would have to update the vertex array with the data every time we render.
         *
         * @return the instanced sphere vao for this mesh
         */

        [[nodiscard]] std::shared_ptr<MeshVertexBuffer> get_mvb() const;

        [[nodiscard]] int get_num_visible_vertices() const;

        [[nodiscard]] int get_num_visible_edges() const;

        [[nodiscard]] std::shared_ptr<VertexArrayObject> get_cylinder_vao() const;

        [[nodiscard]] std::shared_ptr<VertexArrayObject> get_sphere_vao() const;

        [[nodiscard]] int get_id() const
        {
            return m_id;
        }

        [[nodiscard]] std::shared_ptr<OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>> get_ovm() const;

        void translate(glm::vec3 vec);

        void scale(glm::vec3 vec);

        void rotate(float angle, glm::vec3 axis);

        glm::vec3 get_up_direction();

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

        std::shared_ptr<OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>> m_mesh;

        const int key_multiplier = 1000000;

        std::string m_mesh_name = "default";

        int m_max_peel_depth = 0;

        bool m_just_locked;

        std::unordered_set<int> m_selected_vertices;
        std::unordered_set<int> m_selected_edges;
        std::unordered_set<int> m_selected_faces;
        std::unordered_set<int> m_selected_cells;

        std::map<int, int> m_created_shapes;

        std::tuple<int, int> m_selection_offset;

        std::pair<glm::vec3, glm::vec3> m_transformed_bb;

        glm::vec3 m_mesh_offset_from_center;

        glm::vec3 m_slice_dir;

        std::shared_ptr<MeshVertexBuffer> m_mvb = nullptr;

        MeshData m_data;

        int m_id;
    };
}
