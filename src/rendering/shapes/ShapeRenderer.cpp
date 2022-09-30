


#include "ShapeRenderer.h"

#include "../../util/VecUtil.h"
#include "../Renderer.h"
#include "../../util/VecUtil.h"

namespace volumeshOS::Internal
{

    ShapeRenderer::ShapeRenderer()
    {
        m_shape_shader = Shader::get("shape");
    }

    void ShapeRenderer::render(const Renderer& renderer)
    {
        // add all shapes that may have been created or changed
        for (const auto& command : m_on_before_render_commands)
        {
            command();
        }
        m_on_before_render_commands.clear();

        // since we store our data by mesh, we need to remove the shapes of deleted meshes here
        static std::vector<MeshID> removed_meshes;
        removed_meshes.clear();

        auto& camera = renderer.camera;
        auto& light = AppState::settings.light;

        glm::vec3 cam_pos(camera->view * glm::vec4(camera->position, 1.0));
        glm::vec3 light_pos(glm::normalize(light.direction));

        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glEnable(GL_FRAMEBUFFER_SRGB);

        renderer.buffers.target_framebuffer_ms->bind();
        m_shape_shader->bind();

        m_shape_shader->set_uniform_mat4f("u_view", camera->view);
        m_shape_shader->set_uniform_mat4f("u_projection", camera->projection);
        m_shape_shader->set_uniform_vec3f("u_light_pos", light_pos);
        m_shape_shader->set_uniform_vec3f("u_light_color", light.color);
        m_shape_shader->set_uniform_vec3f("u_cam_pos", camera->position);
        m_shape_shader->set_uniform_float("u_gamma", AppState::settings.post_processing.gamma);

        int draw_calls = 0;
        for (auto& [mesh_id, types] : m_render_data_by_mesh_by_type)
        {
            auto mesh_transform = glm::mat4(1.0f);
            if (mesh_id != INVALID_MESH_ID)
            {
                // check if this mesh still exists, and update own data structures accordingly
                auto mesh = renderer.mesh_list->get_mesh(mesh_id);
                if (mesh == nullptr)
                {
                    removed_meshes.push_back(mesh_id);
                    continue;
                }
                // mesh is not visible, so don't render the attached shapes
                if (!Renderer::should_render_mesh(mesh))
                {
                    continue;
                }
                // now, we are sure the mesh exists and is visible, and we can use its transform data
                mesh_transform = mesh->get_data().get_transform();


                // slicing and peeling
                glm::mat4 transform = camera->world * mesh->get_data().get_transform();
                glm::mat4 view_transform = camera->view * transform;
                glm::vec3 view_dir = -glm::normalize(camera->get_front());
                auto slice_direction = mesh->get_slice_dir(transform, view_dir);

                m_shape_shader->set_uniform_float("u_peel_depth", mesh->get_data().peel_level);
                m_shape_shader->set_uniform_float("u_slice_depth", mesh->get_data().slice_level);
                m_shape_shader->set_uniform_vec3f("u_min", mesh->get_world_bb(view_transform).first);
                m_shape_shader->set_uniform_vec3f("u_max", mesh->get_world_bb(view_transform).second);
                m_shape_shader->set_uniform_vec3f("u_slice_direction", slice_direction);
                m_shape_shader->set_uniform_float("u_cell_size", mesh->get_data().cell_size);
                m_shape_shader->set_uniform_float("u_scale_normalization", mesh->get_data().scale_normalization);
            }
            m_shape_shader->set_uniform_mat4f("u_mesh_transform", mesh_transform);
            for (auto& [type, data] : types)
            {
                if (data.needs_update && !data.shapes.empty())
                {
                    update_buffers(data, renderer);
                    data.needs_update = false;
                }
                int num_shapes = static_cast<int>(data.shapes.size());
                data.vao->draw_instanced(num_shapes);
                draw_calls++;
            }
        }

        m_shape_shader->unbind();
        renderer.buffers.target_framebuffer_ms->unbind();

        glDisable(GL_FRAMEBUFFER_SRGB);

        // remove all shapes of meshes that are deleted
        for (const auto& mesh_id : removed_meshes)
        {
            auto& attached_types = m_render_data_by_mesh_by_type[mesh_id];
            for (const auto& [type, data] : attached_types)
            {
                for (const auto& [shape_id, shape] : data.shapes)
                {
                    m_mappings_by_id.erase(shape_id);
                }
            }
            m_render_data_by_mesh_by_type.erase(mesh_id);
        }
    }

    void ShapeRenderer::add_shape(std::unique_ptr<BaseShape>&& shape)
    {
        ShapeID shape_id = shape->id;
        m_shapes_by_id[shape_id] = std::move(shape);
        // defer adding the shape to the next render call, then we can be sure all shapes have up-to-date values
        m_on_before_render_commands.emplace_back([this, shape_id]{
            auto shape_ptr = m_shapes_by_id[shape_id];
            // this shape_ptr was removed before the next render call
            if (shape_ptr == nullptr)
            {
                return;
            }
            if (shape_ptr->cell.is_valid())
            {
                m_shapes_by_cell_id[shape_ptr->cell.idx()].push_back(shape_ptr->id);
            }
            m_mappings_by_id[shape_ptr->id] = std::make_pair(shape_ptr->parent_mesh, shape_ptr->get_hashed_type());
            auto& shapes_of_type = m_render_data_by_mesh_by_type[shape_ptr->parent_mesh][shape_ptr->get_hashed_type()];
            shapes_of_type.shapes[shape_ptr->id] = shape_ptr;
            shapes_of_type.needs_update = true;
        });
    }

    void ShapeRenderer::remove_shape(ShapeID id)
    {
        if (is_valid(id))
        {
            auto& [mesh_id, type] = m_mappings_by_id[id];
            auto& shapes_of_type = m_render_data_by_mesh_by_type[mesh_id][type];
            auto& shape = shapes_of_type.shapes[id];
            if (shape->cell.is_valid())
            {
                auto& shapes = m_shapes_by_cell_id[shape->cell.idx()];
                auto index = std::find(shapes.begin(), shapes.end(), id);
                if (index != shapes.end())
                {
                    shapes.erase(index);
                }
            }
            shapes_of_type.shapes.erase(id);
            shapes_of_type.needs_update = true;
            m_mappings_by_id.erase(id);
            m_shapes_by_id.erase(id);
        }
    }

    void ShapeRenderer::remove_all()
    {
        m_render_data_by_mesh_by_type.clear();
        m_mappings_by_id.clear();
        m_shapes_by_cell_id.clear();
        m_shapes_by_id.clear();
    }

    void ShapeRenderer::update_buffers(ShapeTypeRenderData& data, const Renderer& renderer)
    {
        static ShapeVAOUpdateData update_data;
        update_data.clear();
        for (auto& [id, shape] : data.shapes)
        {
            VecUtil::push_vec3(update_data.positions, shape->position);
            VecUtil::push_vec3(update_data.scales, shape->scale);
            VecUtil::push_vec4(update_data.rotations, shape->rotation);
            VecUtil::push_vec3(update_data.colors, shape->color);
            if (shape->cell.is_valid() && shape->parent_mesh != INVALID_MESH_ID)
            {
                // we can be sure that this mesh was not deleted, this is handled before in render
                auto mvb = renderer.mesh_list->get_mesh(shape->parent_mesh)->get_mvb();
                auto cell_center = mvb->get_cell_center(shape->cell.idx());
                auto cell_peel_depth = mvb->get_cell_peel_depth(shape->cell.idx());
                auto dig_value = mvb->get_cell_dig_value(shape->cell.idx());
                auto isolate_value = mvb->get_cell_isolate_value(shape->cell.idx());
                update_data.has_cell.push_back(1.0f);
                VecUtil::push_vec3(update_data.cell_centers, cell_center);
                update_data.peel_depths.push_back(cell_peel_depth);
                update_data.are_dug.push_back(dig_value);
                update_data.are_isolated.push_back(isolate_value);
            }
            else
            {
                update_data.has_cell.push_back(0.0f);
                VecUtil::push_vec3(update_data.cell_centers, glm::vec3{0.0f, 0.0f, 0.0f});
                update_data.peel_depths.push_back(0.0f);
                update_data.are_dug.push_back(0.0f);
                update_data.are_isolated.push_back(0.0f);
            }
        }
        if (data.vao == nullptr)
        {
            // we can be sure there is at least one element
            data.vao = data.shapes.begin()->second->get_vao();
            data.vao->add_attribute(update_data.positions, 2, 3, true);
            data.vao->add_attribute(update_data.scales, 3, 3, true);
            data.vao->add_attribute(update_data.rotations, 4, 4, true);
            data.vao->add_attribute(update_data.colors, 5, 3, true);
            data.vao->add_attribute(update_data.has_cell, 6, 1, true);
            data.vao->add_attribute(update_data.cell_centers, 7, 3, true);
            data.vao->add_attribute(update_data.peel_depths, 8, 1, true);
            data.vao->add_attribute(update_data.are_dug, 9, 1, true);
            data.vao->add_attribute(update_data.are_isolated, 10, 1, true);
        }
        else
        {
            data.vao->update_attribute(update_data.positions, 2);
            data.vao->update_attribute(update_data.scales, 3);
            data.vao->update_attribute(update_data.rotations, 4);
            data.vao->update_attribute(update_data.colors, 5);
            data.vao->update_attribute(update_data.are_dug, 9);
            data.vao->update_attribute(update_data.are_isolated, 10);
        }
    }

    void ShapeRenderer::set_position(ShapeID id, float x, float y, float z)
    {
        auto shape = get_shape(id);
        if (shape != nullptr)
        {
            shape->position = {x, y, z};
            m_on_before_render_commands.emplace_back([this, id]{
                auto& [mesh_id, type] = m_mappings_by_id[id];
                m_render_data_by_mesh_by_type[mesh_id][type].needs_update = true;
            });
        }
    }

    glm::vec3 ShapeRenderer::get_position(ShapeID id)
    {
        auto shape = get_shape(id);
        if (shape != nullptr)
        {
            return shape->position;
        }
        return { 0.0f, 0.0f, 0.0f };
    }

    void ShapeRenderer::set_scale(ShapeID id, float x, float y, float z)
    {
        auto shape = get_shape(id);
        if (shape != nullptr)
        {
            shape->scale = {x, y, z};
            m_on_before_render_commands.emplace_back([this, id]{
                auto& [mesh_id, type] = m_mappings_by_id[id];
                m_render_data_by_mesh_by_type[mesh_id][type].needs_update = true;
            });
        }
    }

    glm::vec3 ShapeRenderer::get_scale(ShapeID id)
    {
        auto shape = get_shape(id);
        if (shape != nullptr)
        {
            return shape->scale;
        }
        return { 0.0f, 0.0f, 0.0f };
    }

    void ShapeRenderer::set_rotation(ShapeID id, float x, float y, float z, float angle)
    {
        auto shape = get_shape(id);
        if (shape != nullptr)
        {
            shape->rotation = {x, y, z, angle};
            m_on_before_render_commands.emplace_back([this, id]{
                auto& [mesh_id, type] = m_mappings_by_id[id];
                m_render_data_by_mesh_by_type[mesh_id][type].needs_update = true;
            });
        }
    }

    glm::vec4 ShapeRenderer::get_rotation(ShapeID id)
    {
        auto shape = get_shape(id);
        if (shape != nullptr)
        {
            return shape->rotation;
        }
        return { 0.0f, 0.0f, 0.0f, 0.0f };
    }

    void ShapeRenderer::set_color(ShapeID id, float r, float g, float b)
    {
        auto shape = get_shape(id);
        if (shape != nullptr)
        {
            shape->color = {r, g, b};
            m_on_before_render_commands.emplace_back([this, id]{
                auto& [mesh_id, type] = m_mappings_by_id[id];
                m_render_data_by_mesh_by_type[mesh_id][type].needs_update = true;
            });
        }
    }

    glm::vec3 ShapeRenderer::get_color(ShapeID id)
    {
        auto shape = get_shape(id);
        if (shape != nullptr)
        {
            return shape->color;
        }
        return { 0.0f, 0.0f, 0.0f };
    }

    bool ShapeRenderer::is_valid(ShapeID id) const
    {
        return id >= 0 && m_shapes_by_id.find(id) != m_shapes_by_id.end();
    }

    BaseShape* ShapeRenderer::get_shape(ShapeID id)
    {
        if (is_valid(id))
        {
            return m_shapes_by_id[id].get();
        }
        Log::warn("Attempt to access invalid shape: " + std::to_string(id));
        return nullptr;
    }

    void ShapeRenderer::update_cell(int cell_id)
    {
        m_on_before_render_commands.emplace_back([this, cell_id]{
            // check if shapes for this cell exists
            if (m_shapes_by_cell_id.find(cell_id) != m_shapes_by_cell_id.end())
            {
                auto& shapes_of_cell = m_shapes_by_cell_id[cell_id];
                for (const auto& shape_id : shapes_of_cell)
                {
                    auto shape = get_shape(shape_id);
                    if (shape != nullptr)
                    {
                        auto& [mesh_id, type] = m_mappings_by_id[shape_id];
                        m_render_data_by_mesh_by_type[mesh_id][type].needs_update = true;
                    }
                }
            }
        });
    }

    void ShapeRenderer::reset_visibility()
    {
        m_on_before_render_commands.emplace_back([this]{
            for (const auto& [cell_id, shapes_of_cell] : m_shapes_by_cell_id)
            {
                for (const auto& shape_id : shapes_of_cell)
                {
                    auto shape = get_shape(shape_id);
                    if (shape != nullptr)
                    {
                        auto& [mesh_id, type] = m_mappings_by_id[shape_id];
                        m_render_data_by_mesh_by_type[mesh_id][type].needs_update = true;
                    }
                }
            }
        });
    }
}