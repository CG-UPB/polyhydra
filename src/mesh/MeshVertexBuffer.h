#pragma once

#include <OpenVolumeMesh/Core/GeometryKernel.hh>
#include <OpenVolumeMesh/Attribs/NormalAttrib.hh>
#include "../rendering/gl/VertexArrayObject.h"
#include "glm/gtx/transform.hpp"
#include <map>
#include <unordered_map>
#include <chrono>

namespace vOS
{
    typedef OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d> Mesh;
    typedef OpenVolumeMesh::CellHandle Cell;

    struct VertexData
    {
        glm::vec3 position{};
        glm::vec3 normal{};
        glm::vec3 color{};
    };

    struct HalffaceData
    {
        std::vector<VertexData> vertices;
        std::vector<unsigned int> indices;
        std::vector<int> halfface_ids;
    };

    struct RoundedVertexData
    {
        int from_vertex_id = -1;
        int to_vertex_id = -1;
        int halfedge_id = -1;
        int halfface_id = -1;
    };

    struct RoundedFaceVertexData
    {
        unsigned int index = -1;
        int corner_vertex_id = -1;
        int to_vertex_id = -1;
        int next_to_vertex_id = -1;
        int to_vertex_halfedge_id = -1;
        int next_to_vertex_halfedge_id = -1;
    };

    struct VertexType {
        static constexpr const float FACE    = 0.0f;
        static constexpr const float EDGE    = 1.0f;
        static constexpr const float CORNER  = 2.0f;
        static constexpr const float CENTER  = 3.0f;
    };

    struct RoundedCellData
    {
        int cell_id = -1;
        std::vector<float> vertex_types;
        std::vector<float> vertex_positions;
        std::vector<float> vertex_normals;
        std::vector<float> vertex_cell_centers;
        std::vector<float> vertex_colors;
        std::vector<float> vertex_peel_depths;
        std::vector<float> vertex_is_triangle;
        std::vector<float> vertex_is_digged;
        std::vector<float> vertex_is_isolated;
        std::vector<float> face_center;
        std::vector<float> to_vertex;
        std::vector<float> dihedral_angle;
        std::vector<float> selection;
        std::vector<float> hovered;
        std::vector<unsigned int> indices;
    };

    struct AttributeData
    {
        int location = -1;
        int element_count = -1;
        bool per_instance = false;
    };

    enum class VAO
    {
        MESH_FACE, MESH_ROUNDED, SPHERE, CYLINDER,
        NUM_VALUES
    };

    enum class Attribute
    {
        POSITION, NORMAL, CELL_CENTER, PEEL_DEPTH, IS_DIGGED, COLOR, IS_ISOLATED, IS_TRIANGLE, VERTEX_TYPE,
        FACE_CENTER, TO_VERTEX, DIHEDRAL_ANGLE, SELECTION, SELECTION_VERTEX_POSITION, SELECTION_FROM_VERTEX,
        SELECTION_TO_VERTEX, HOVERED,
        NUM_VALUES
    };

    struct AttributeDefinitions
    {
        AttributeDefinitions();
        std::unordered_map<int, AttributeData>& of(const VAO vao)
        {
            return m_locations[static_cast<int>(vao)];
        }
    private:
        void define_attribute(Attribute attribute, const AttributeData& data, std::initializer_list<VAO> vaos)
        {
            for (const VAO& vao : vaos)
            {
                m_locations[static_cast<int>(vao)][static_cast<int>(attribute)] = data;
            }
        }
        std::unordered_map<int, std::unordered_map<int, AttributeData>> m_locations{};
    };

    struct AttributeUpdateData
    {
        VAO vao;
        Attribute attribute;
        glm::vec4 value;
        int value_size;
        int halfface_id;
        int cell_id;
    };

    class MeshVertexBuffer
    {
    public:

        explicit MeshVertexBuffer(Mesh* mesh);

        ~MeshVertexBuffer();

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

        [[nodiscard]] int get_num_selection_vertices() const;

        [[nodiscard]] int get_num_selection_edges() const;

        std::vector<float>& get_original_vertices();

        VertexArrayObject* get_vao_by_face();

        VertexArrayObject* get_vao_rounded();

        VertexArrayObject* get_vertex_only_vao();

        [[nodiscard]] float get_average_cell_size() const;

        /**
         * Colors the given Face in desired colors
         * The strong a is, the more pronounced the given color is
         * An Alpha value of 0 will have no visible effect, 1 will completely override the object default color
         * @param face_id
         * @param r
         * @param g
         * @param b
         * @param a
         */
        void set_face_color(int face_id, float r, float g, float b, float a);

        /**
         * Select or unselect the given Face
         * @param face_id
         * @param selected
         */
        void set_face_selection(int face_id, bool selected);

        void set_halfface_color(int halfface_id, float r, float g, float b, float a);

        void set_halfface_selection(int halfface_id, bool selected);

        void hover_halfface(int halfface_id);

        void set_cell_color(int cell_id, float r, float g, float b, float a);

        void set_cell_selection(int cell_id, bool selected);

        void hover_cell(int cell_id);

        VertexArrayObject* get_sphere_vao();

        VertexArrayObject* get_cylinder_vao();

        void update_digging_buffer(int id, float newValue);

        void update_isolate_buffer(int id, float newValue);

        void reset_isolation();

        void reset_digging();

        void activate_isolation();

        void start_isolation();

        glm::vec3 get_face_normal(int ovm_id)
        { return m_face_normals[ovm_id]; }

        glm::vec3 get_halfface_barycenter(int ovm_id)
        { return m_face_centers[ovm_id]; }

        glm::vec3 get_min_bounding_box(){return min_bounding_box;}
        glm::vec3 get_max_bounding_box(){return max_bounding_box;}
        void load_next_cell();

        bool is_loading_finished() const;

        float get_loading_percentage();

        void reset_hover();

    private:

        // vertex attributes
        static AttributeDefinitions s_attribute_definitions;

        void build_vertex_arrays();

        void add_cell_rounded(Mesh& mesh, Cell cell);

        unsigned int add_vertex_data_to_cell_data(
                RoundedCellData& data,
                float type,
                const glm::vec3& pos,
                const glm::vec3& norm,
                const glm::vec4& col,
                const glm::vec3& face_center,
                const glm::vec3& to_vertex,
                float dihedral_angle
        );

        void add_cell_triangle_indices(RoundedCellData& data, unsigned int i0, unsigned int i1, unsigned int i2) const;

        void add_cell_by_faces(Mesh& mesh, Cell cell);

        void add_face_indices(Mesh& mesh, HalffaceData& face) const;

        void add_from_to_vertex(Mesh& mesh, const OpenVolumeMesh::VertexHandle& from,
                                const OpenVolumeMesh::VertexHandle& to);

        static std::vector<float> get_vertices(Mesh& mesh);

        [[nodiscard]] inline glm::vec3 normal_to_vec3(int halfface_id);

        void add_attribute_data(VAO vao, Attribute attribute, const std::vector<float>& data);

        std::vector<float>& get_attrib_array(VAO vao, Attribute attribute);

        void add_vao_attributes(VertexArrayObject* vao, VAO vao_id);

        std::pair<int, int>& get_halfface_index_and_count(VAO vao, int halfface_id);

        void add_halfface_index_and_count(VAO vao, int halfface_id, int index, int count);

        std::pair<int, int>& get_cell_index_and_count(VAO vao, int cell_id);

        void add_cell_index_and_count(VAO vao, int cell_id, int index, int count);

        template<typename T>
        void update_halfface_attribute(VAO vao_id, Attribute attribute, int halfface_id, T data);

        template<typename T>
        void update_cell_attribute(VAO vao_id, Attribute attribute, int cell_id, T data);

        void set_attribute_buffer(std::vector<float>& buffer, size_t offset, int value_size, const glm::vec4& value);

        template<typename T>
        void update_attribute(VAO vao_id, Attribute attribute, T value, int halfface_id = -1, int cell_id = -1);

        template<typename T>
        std::pair<glm::vec4, int> get_value_and_size(T value);

        void update_vertex_arrays();

        // ovm references
        Mesh& m_mesh;
        OpenVolumeMesh::NormalAttrib<Mesh> m_normals;

        // loading stats
        int m_num_loaded_cells = 0;
        bool m_is_loading_finished = false;
        OpenVolumeMesh::CellIter m_current_loading_cell_it;
        std::chrono::steady_clock::time_point m_loading_start;

        // vertex arrays
        struct
        {
            VertexArrayObject* face = nullptr;
            VertexArrayObject* rounded = nullptr;
            VertexArrayObject* selection_sphere = nullptr;
            VertexArrayObject* selection_cylinder = nullptr;
            VertexArrayObject* vertex_only = nullptr;
        } m_vao;

        // ovm ids, in the order that we render them
        struct
        {
            std::vector<int> vertex_ids;
            std::vector<int> edge_ids;
            std::vector<int> halfface_ids;
        } m_selection_map;

        // to be used for rounded cells as well, no need to calculate twice
        std::unordered_map<int, glm::vec3> m_cell_centers;
        std::unordered_map<int, float> m_peel_depths;

        struct VertexAttributeMap
        {
            std::unordered_map<int, std::vector<float>> data;
            std::unordered_map<int, std::pair<int, int>> halfface_index_and_count;
            std::unordered_map<int, std::pair<int, int>> cell_index_and_count;
        };

        std::vector<AttributeUpdateData> m_vao_update_data;
        std::unordered_map<int, VertexAttributeMap> m_attributes{};
        std::unordered_map<int, std::vector<VertexArrayObject*>> m_vertex_arrays{};

        std::vector<float> m_original_vertices;
        std::vector<unsigned int> m_indices_face;
        std::vector<unsigned int> m_indices_rounded;

        int m_current_rounded_index = 0;
        int m_num_vertices_face = 0;
        int m_vertex_offset_face = 0;
        int m_vertex_offset_rounded = 0;
        float m_average_cell_size = 0.0f;

        int m_current_hovered_halfface_id = -1;
        int m_current_hovered_cell_id = -1;

        // Saved Data
        std::map<int, glm::vec3> m_face_normals;
        std::map<int, glm::vec3> m_face_centers;
        glm::vec3 min_bounding_box;
        glm::vec3 max_bounding_box;
    };
}