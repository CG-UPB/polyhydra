#pragma once

#include "vospch.h"

#include "../gl/VertexArrayObject.h"
#include "../gl/Shader.h"
#include "../passes/RenderPass.h"
#include "Shapes.h"

namespace volumeshOS::Internal
{

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
        std::unordered_map<ShapeID, std::shared_ptr<BaseShape>> shapes  = {};
        std::unique_ptr<VertexArrayObject> vao                          = nullptr;
        bool needs_update                                               = true;
    };

    using ShapeMap = std::unordered_map<MeshID, std::unordered_map<ShapeType, ShapeTypeRenderData>>;

    class ShapeRenderer : public RenderPass
    {
    public:

        ShapeRenderer();

        void render(const Renderer& renderer) override;

        void add_shape(std::unique_ptr<BaseShape>&& shape);

        void remove_shape(ShapeID id);

        void remove_all();

        void set_position(ShapeID id, float x, float y, float z);

        [[nodiscard]] glm::vec3 get_position(ShapeID id);

        void set_scale(ShapeID id, float x, float y, float z);

        [[nodiscard]] glm::vec3 get_scale(ShapeID id);

        void set_rotation(ShapeID id, float x, float y, float z, float angle);

        [[nodiscard]] glm::vec4 get_rotation(ShapeID id);

        void set_color(ShapeID id, float r, float g, float b);

        [[nodiscard]] glm::vec3 get_color(ShapeID id);

        [[nodiscard]] bool is_valid(ShapeID id) const;

        void update_cell(int cell_id);

        void reset_visibility();

        template<typename T>
        [[nodiscard]] T* get_shape_and_update_buffers(ShapeID id)
        {
            auto shape_ptr = get_shape(id);
            if (shape_ptr != nullptr)
            {
                m_on_before_render_commands.emplace_back([this, id]{
                    auto& [mesh_id, type] = m_mappings_by_id[id];
                    auto shape = get_shape(id);
                    if (shape != nullptr)
                    {
                        m_render_data_by_mesh_by_type[mesh_id][type].needs_update = true;
                    }
                });
            }
            return dynamic_cast<T*>(shape_ptr);
        }

    private:
        static void update_buffers(ShapeTypeRenderData& data, const Renderer& renderer);

    private:
        [[nodiscard]] BaseShape* get_shape(ShapeID id);

    private:

        std::shared_ptr<Shader> m_shape_shader                                      = nullptr;
        std::unordered_map<ShapeID, std::shared_ptr<BaseShape>> m_shapes_by_id      = {};
        std::vector<std::function<void()>> m_on_before_render_commands              = {};
        std::unordered_map<ShapeID, std::pair<MeshID, ShapeType>> m_mappings_by_id  = {};
        std::unordered_map<int, std::vector<ShapeID>> m_shapes_by_cell_id           = {};
        ShapeMap m_render_data_by_mesh_by_type                                      = {};
    };
}