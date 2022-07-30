#pragma once

#include "vospch.h"

#include "../gl/VertexArrayObject.h"
#include "../gl/Shader.h"
#include "../passes/RenderPass.h"

namespace volumeshOS::Internal
{
    struct ShapeDefinition
    {
        ShapeType type          = ShapeType::SPHERE;
        glm::vec3 position      = {0.0f, 0.0f, 0.0f};
        glm::vec3 scale         = {1.0f, 1.0f, 1.0f};
        // default rotation is 0 deg around y-axis
        glm::vec4 rotation      = {0.0f, 1.0f, 0.0f, 0.0f};
        glm::vec3 color         = {0.2f, 0.2f, 1.0f};
        ShapeID id              = INVALID_SHAPE_ID;
        MeshID parent_mesh      = INVALID_MESH_ID;
        int cell_id             = -1;
        float is_dug            = 0.0f;
        float is_isolated       = 0.0f;
    };

    struct ShapeVAOUpdateData
    {
        std::vector<float> positions        = {};   // location = 2
        std::vector<float> scales           = {};   // location = 3
        std::vector<float> rotations        = {};   // location = 4
        std::vector<float> colors           = {};   // location = 5
        std::vector<float> has_cell         = {};   // location = 6
        std::vector<float> cell_centers     = {};   // location = 7
        std::vector<float> peel_depths      = {};   // location = 8
        std::vector<float> are_dug          = {};   // location = 9
        std::vector<float> are_isolated     = {};   // location = 10

        void clear()
        {
            positions.clear();
            scales.clear();
            rotations.clear();
            colors.clear();
            cell_centers.clear();
            peel_depths.clear();
            are_dug.clear();
            are_isolated.clear();
        }
    };

    struct ShapeTypeRenderData
    {
        std::unordered_map<ShapeID, ShapeDefinition> shapes     = {};
        std::unique_ptr<VertexArrayObject> vao                  = nullptr;
        bool needs_update                                       = true;
    };

    using ShapeMap = std::unordered_map<MeshID, std::unordered_map<ShapeType, ShapeTypeRenderData>>;

    class ShapeRenderer : public RenderPass
    {
    public:

        ShapeRenderer();

        void render(const Renderer& renderer) override;

        void add_shape(ShapeDefinition& shape);

        void remove_shape(ShapeID id);

        void remove_all();

        void set_position(ShapeID id, float x, float y, float z);

        [[nodiscard]] glm::vec3 get_position(ShapeID id);

        void set_scale(ShapeID id, float x, float y, float z);

        [[nodiscard]] glm::vec3 get_scale(ShapeID id);

        void set_rotation(ShapeID id, float x, float y, float z);

        [[nodiscard]] glm::vec4 get_rotation(ShapeID id);

        void set_color(ShapeID id, float r, float g, float b);

        [[nodiscard]] glm::vec3 get_color(ShapeID id);

        [[nodiscard]] bool is_valid(ShapeID id) const;

        void set_dig(int cell_id, float dig);

        void set_isolate(int cell_id, float isolate);

        void reset_visibility();

    private:

        static void update_buffers(ShapeType type, ShapeTypeRenderData& data, const Renderer& renderer);

        [[nodiscard]] static std::unique_ptr<VertexArrayObject> get_vao_for_type(ShapeType type);

    private:

        [[nodiscard]] std::tuple<MeshID, ShapeType, ShapeDefinition*> get_shape(ShapeID id);

    private:

        std::shared_ptr<Shader> m_shape_shader                                      = nullptr;
        std::unordered_map<ShapeID, std::pair<MeshID, ShapeType>> m_mappings_by_id  = {};
        std::unordered_map<int, std::vector<ShapeID>> m_shapes_by_cell_id           = {};
        ShapeMap m_render_data_by_mesh_by_type                                      = {};
    };
}