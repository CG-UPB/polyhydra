#pragma once

#include "vospch.h"

#include "../rendering/gl/VertexArrayObject.h"
#include "MeshVertexBuffer.h"
#include "MeshTextureBuffer.h"
#include "../util/VecUtil.h"
#include "../util/Enums.h"

#include<glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>
#include<glm/common.hpp>



namespace volumeshOS::Internal
{
    class MeshSerializer;

    struct MeshData
    {

        [[nodiscard]] const glm::mat4& get_transform() const
        {
            return transformation;
        }

        void update_transform();

        glm::mat4 rotation          = glm::mat4(1.0f);
        glm::mat4 transformation    = glm::mat4(1.0f);
        glm::quat rot               = glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 0.0f));

        // Rendering Variables
        RenderingMode rendering_mode    = RenderingMode::CELLS;
        bool points                     = false;
        bool lines                      = false;
        bool cells                      = true;

        glm::vec4 point_color = {0.8f, 0.8f, 0.8f, 0.8f};
        glm::vec4 line_color = {0.8f, 0.8f, 0.8f, 0.8f};

        ShadingMode shading_mode        = ShadingMode::FLAT;
        float line_width                = 1.2f;
        float point_size                = 0.4f;

        glm::vec4 color             = {1.0f, 1.0f, 1.0f, 1.0f};
        glm::vec4 selection_color   = {1.0f, 1.0f, 1.0f, 0.0f};

        // phong lighting
        float ambient_strength      = 1.0f;
        float diffuse_strength      = 1.0f;
        float specular_strength     = 0.15f;
        float specular_exponent     = 8.0f;

        // pbr lighting
        bool use_pbr                = true;
        float metallic              = 0.15;
        float roughness             = 0.65;

        // Toolbox Variables
        float peel_level            = 0.0f;
        float max_peel_depth        = 0.0f;
        bool reverse_peeling        = false;
        float slice_level           = 0.0f;
        float cell_size             = 1.0f;
        bool slice_locked           = false;
        float rounding_size         = 0.0f;
        int tessellation_level      = 20;

        // Transform Variables
        glm::vec3 position          = {0.0f, 0.0f, 0.0f};
        glm::vec3 scale             = {1.0f, 1.0f, 1.0f};
        glm::vec3 position_offset   = {0.0f, 0.0f, 0.0f};
        float scale_normalization   = 1.0f;

        // Other
        bool visible                = true;
        int selection_id_offset     = 0;
        std::string name            = "default";
        bool use_two_sided_lighting = false;
        bool use_back_face_culling  = true;
        bool use_base_color         = true;
    };

    class MeshObject
    {
    public:

        explicit MeshObject(int id);

        void set_mesh(const std::shared_ptr<OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>>& mesh);

        /**
         * Calculate the amount of needed selection ids
         * @param start id where ids start
         */
        void set_selection_offset(int start);

        void set_halfface_color(int ovm_id, const glm::vec4& color);

        void set_face_color(int ovm_id, const glm::vec4& color);

        void set_cell_color(int ovm_id, const glm::vec4& color);

        void set_mesh_color(const glm::vec4& color);

        /**
         * updates the vertex_buffer
         */
        void update_vertex_buffer();

        void update_texture_buffer();

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


        [[nodiscard]] float get_max_peel_depth() const;

        const std::array<int, 2>& selection_offset()
        {
            return m_selection_offset;
        };

        [[nodiscard]] std::shared_ptr<VertexArrayObject> get_vao() const;

        glm::vec3 get_min();

        glm::vec3 get_max();

        std::pair<glm::vec3, glm::vec3> &get_world_bb(const glm::mat4 &transform);

        glm::vec3 &get_slice_dir(const glm::mat4 &world_transform, const glm::vec3 &view_dir);

        MeshData& get_data()
        {
            return m_data;
        }

        void set_data(const MeshData& data)
        {
            m_data = data;
        }

        [[nodiscard]] std::shared_ptr<MeshVertexBuffer> get_mvb() const;

        [[nodiscard]] int get_num_visible_vertices() const;

        [[nodiscard]] int get_num_visible_edges() const;

        [[nodiscard]] std::shared_ptr<VertexArrayObject> get_cylinder_vao() const;

        [[nodiscard]] std::shared_ptr<VertexArrayObject> get_sphere_vao() const;

        [[nodiscard]] std::shared_ptr<MeshTextureBuffer> get_mtb() const;

        [[nodiscard]] int get_id() const
        {
            return m_id;
        }

        [[nodiscard]] std::shared_ptr<OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>> get_ovm() const;

        [[nodiscard]] bool is_bezier_mesh() const;

        void translate(const glm::vec3& vec);

        void scale(const glm::vec3& vec);

        void rotate(float angle, const glm::vec3& axis);

        void rotate_axis(float x, float y, float z);

        void reset_rotation();

    public:
        bool just_locked    = false;
        bool roundings_loaded = false;

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

    private:

        std::shared_ptr<OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>> m_mesh   = nullptr;
        std::array<int, 2> m_selection_offset               = {-1, -1};
        std::pair<glm::vec3, glm::vec3> m_transformed_bb    = {};
        glm::vec3 m_slice_dir                               = {0.0f, 0.0f, 0.0f};
        std::shared_ptr<MeshVertexBuffer> m_mvb             = nullptr;
        std::shared_ptr<MeshTextureBuffer> m_mtb            = nullptr;
        MeshData m_data                                     = {};
        int m_id                                            = -1;

        friend class MeshSerializer;
    };
}
