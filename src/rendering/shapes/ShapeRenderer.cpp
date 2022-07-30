
#include "ShapeRenderer.h"

#include "../../util/VecUtil.h"
#include "../meshes/CommonMeshes.h"
#include "../Renderer.h"

namespace volumeshOS::Internal
{

    ShapeRenderer::ShapeRenderer()
    {
        m_shape_shader = Shader::get("shape");
    }

    void ShapeRenderer::render(const Renderer& renderer)
    {
        // since we store our data by mesh, we need to remove the shapes of deleted meshes here
        static std::vector<MeshID> removed_meshes;
        removed_meshes.clear();

        auto& camera = renderer.camera;
        auto& light = renderer.light;

        glm::vec3 cam_pos(camera->view * glm::vec4(camera->position, 1.0));
        glm::mat3 mvp_ti = glm::mat3(glm::transpose(glm::inverse(camera->view)));
        glm::vec3 light_pos(glm::normalize(mvp_ti * light.light_dir));

        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);

        renderer.buffers.target_framebuffer_ms->bind();
        m_shape_shader->bind();

        m_shape_shader->set_uniform_mat4f("u_view", camera->view);
        m_shape_shader->set_uniform_mat4f("u_projection", camera->projection);
        m_shape_shader->set_uniform_vec3f("u_light_pos", light_pos);
        m_shape_shader->set_uniform_vec3f("u_light_color", light.color);
        m_shape_shader->set_uniform_vec3f("u_cam_pos", camera->position);

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
                glm::mat4 l_transform = light.world * mesh->get_data().get_transform();
                glm::mat4 view_transform = camera->view * transform;
                glm::vec3 view_dir = -glm::normalize(camera->get_front());
                auto slice_direction = mesh->get_slice_dir(transform, view_dir);

                m_shape_shader->set_uniform_float("u_peel_depth", mesh->get_data().peel_level);
                m_shape_shader->set_uniform_float("u_slice_depth", mesh->get_data().slice_level);
                m_shape_shader->set_uniform_vec3f("u_min", mesh->get_world_bb(view_transform).first);
                m_shape_shader->set_uniform_vec3f("u_max", mesh->get_world_bb(view_transform).second);
                m_shape_shader->set_uniform_vec3f("u_slice_direction", slice_direction);
                m_shape_shader->set_uniform_float("u_cell_size", mesh->get_data().cell_size);
            }
            m_shape_shader->set_uniform_mat4f("u_mesh_transform", mesh_transform);
            for (auto& [type, data] : types)
            {
                if (data.needs_update)
                {
                    update_buffers(type, data, renderer);
                    data.needs_update = false;
                }
                int num_shapes = static_cast<int>(data.shapes.size());
                data.vao->draw_instanced(num_shapes);
            }
        }

        m_shape_shader->unbind();
        renderer.buffers.target_framebuffer_ms->unbind();

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

    void ShapeRenderer::add_shape(ShapeDefinition& shape)
    {
        if (shape.cell_id != -1)
        {
            m_shapes_by_cell_id[shape.cell_id].push_back(shape.id);
        }
        m_mappings_by_id[shape.id] = std::make_pair(shape.parent_mesh, shape.type);
        auto& shapes_of_type = m_render_data_by_mesh_by_type[shape.parent_mesh][shape.type];
        shapes_of_type.shapes[shape.id] = shape;
        shapes_of_type.needs_update = true;
    }

    void ShapeRenderer::remove_shape(ShapeID id)
    {
        if (is_valid(id))
        {
            auto& [mesh_id, type] = m_mappings_by_id[id];
            auto& shapes_of_type = m_render_data_by_mesh_by_type[mesh_id][type];
            auto& shape = shapes_of_type.shapes[id];
            if (shape.cell_id != -1)
            {
                auto& shapes = m_shapes_by_cell_id[shape.cell_id];
                auto index = std::find(shapes.begin(), shapes.end(), id);
                if (index != shapes.end())
                {
                    shapes.erase(index);
                }
            }
            shapes_of_type.shapes.erase(id);
            shapes_of_type.needs_update = true;
            m_mappings_by_id.erase(id);
        }
    }

    void ShapeRenderer::remove_all()
    {
        m_render_data_by_mesh_by_type.clear();
        m_mappings_by_id.clear();
        m_shapes_by_cell_id.clear();
    }

    void ShapeRenderer::update_buffers(ShapeType type, ShapeTypeRenderData& data, const Renderer& renderer)
    {
        static ShapeVAOUpdateData update_data;
        update_data.clear();
        for (auto& [id, shape] : data.shapes)
        {
            VecUtil::push_vec3(update_data.positions, shape.position);
            VecUtil::push_vec3(update_data.scales, shape.scale);
            VecUtil::push_vec4(update_data.rotations, shape.rotation);
            VecUtil::push_vec3(update_data.colors, shape.color);
            if (shape.cell_id != -1 && shape.parent_mesh != INVALID_MESH_ID)
            {
                // we can be sure that this mesh was not deleted, this is handled before in render
                auto mvb = renderer.mesh_list->get_mesh(shape.parent_mesh)->get_mvb();
                auto cell_center = mvb->get_cell_center(shape.cell_id);
                auto cell_peel_depth = mvb->get_cell_peel_depth(shape.cell_id);
                auto dig_value = mvb->get_cell_dig_value(shape.cell_id);
                auto isolate_value = mvb->get_cell_isolate_value(shape.cell_id);
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
            data.vao = get_vao_for_type(type);
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

    std::unique_ptr<VertexArrayObject> ShapeRenderer::get_vao_for_type(ShapeType type)
    {
        switch (type)
        {
            case ShapeType::BOX:
            {
                auto vao = std::make_unique<VertexArrayObject>(
                        CommonMeshes::Box::vertices(),
                        CommonMeshes::Box::indices()
                );
                vao->add_attribute(CommonMeshes::Box::normals(), 1, 3, false);
                return vao;
            }
            case ShapeType::CYLINDER:
            {
                auto vao = std::make_unique<VertexArrayObject>(
                        CommonMeshes::Cylinder::vertices(),
                        CommonMeshes::Cylinder::indices()
                );
                vao->add_attribute(CommonMeshes::Cylinder::normals(), 1, 3, false);
                return vao;
            }
            case ShapeType::SPHERE:
            {
                auto vao = std::make_unique<VertexArrayObject>(
                        CommonMeshes::Sphere::vertices(),
                        CommonMeshes::Sphere::indices()
                );
                vao->add_attribute(CommonMeshes::Sphere::normals(), 1, 3, false);
                return vao;
            }
        }
        return nullptr;
    }

    void ShapeRenderer::set_position(ShapeID id, float x, float y, float z)
    {
        auto [mesh_id, type, shape] = get_shape(id);
        if (shape != nullptr)
        {
            shape->position = {x, y, z};
            m_render_data_by_mesh_by_type[mesh_id][type].needs_update = true;
        }
    }

    glm::vec3 ShapeRenderer::get_position(ShapeID id)
    {
        auto [mesh_id, type, shape] = get_shape(id);
        if (shape != nullptr)
        {
            return shape->position;
        }
        return { 0.0f, 0.0f, 0.0f };
    }

    void ShapeRenderer::set_scale(ShapeID id, float x, float y, float z)
    {
        auto [mesh_id, type, shape] = get_shape(id);
        if (shape != nullptr)
        {
            shape->scale = {x, y, z};
            m_render_data_by_mesh_by_type[mesh_id][type].needs_update = true;
        }
    }

    glm::vec3 ShapeRenderer::get_scale(ShapeID id)
    {
        auto [mesh_id, type, shape] = get_shape(id);
        if (shape != nullptr)
        {
            return shape->scale;
        }
        return { 0.0f, 0.0f, 0.0f };
    }

    void ShapeRenderer::set_rotation(ShapeID id, float x, float y, float z)
    {
        auto [mesh_id, type, shape] = get_shape(id);
        if (shape != nullptr)
        {
            shape->rotation = {x, y, z, 0.0f};
            m_render_data_by_mesh_by_type[mesh_id][type].needs_update = true;
        }
    }

    glm::vec4 ShapeRenderer::get_rotation(ShapeID id)
    {
        auto [mesh_id, type, shape] = get_shape(id);
        if (shape != nullptr)
        {
            return shape->rotation;
        }
        return { 0.0f, 0.0f, 0.0f, 0.0f };
    }

    void ShapeRenderer::set_color(ShapeID id, float r, float g, float b)
    {
        auto [mesh_id, type, shape] = get_shape(id);
        if (shape != nullptr)
        {
            shape->color = {r, g, b};
            m_render_data_by_mesh_by_type[mesh_id][type].needs_update = true;
        }
    }

    glm::vec3 ShapeRenderer::get_color(ShapeID id)
    {
        auto [mesh_id, type, shape] = get_shape(id);
        if (shape != nullptr)
        {
            return shape->color;
        }
        return { 0.0f, 0.0f, 0.0f };
    }

    bool ShapeRenderer::is_valid(ShapeID id) const
    {
        return id >= 0 && m_mappings_by_id.find(id) != m_mappings_by_id.end();
    }

    std::tuple<MeshID, ShapeType, ShapeDefinition*> ShapeRenderer::get_shape(ShapeID id)
    {
        if (is_valid(id))
        {
            auto& [mesh_id, type] = m_mappings_by_id[id];
            auto* shape = &m_render_data_by_mesh_by_type[mesh_id][type].shapes[id];
            return { mesh_id, type, shape };
        }
        Log::warn("Attempt to access invalid shape: " + std::to_string(id));
        return { INVALID_MESH_ID, ShapeType::BOX, nullptr };
    }

    void ShapeRenderer::set_dig(int cell_id, float dig)
    {
        // check if shapes for this cell exists
        if (m_shapes_by_cell_id.find(cell_id) != m_shapes_by_cell_id.end())
        {
            auto& shapes_of_cell = m_shapes_by_cell_id[cell_id];
            for (const auto& shape_id : shapes_of_cell)
            {
                auto [mesh_id, type, shape] = get_shape(shape_id);
                if (shape != nullptr)
                {
                    shape->is_dug = dig;
                    m_render_data_by_mesh_by_type[mesh_id][type].needs_update = true;
                }
            }
        }
    }

    void ShapeRenderer::set_isolate(int cell_id, float isolate)
    {
        // check if shapes for this cell exists
        if (m_shapes_by_cell_id.find(cell_id) != m_shapes_by_cell_id.end())
        {
            auto& shapes_of_cell = m_shapes_by_cell_id[cell_id];
            for (const auto& shape_id : shapes_of_cell)
            {
                auto [mesh_id, type, shape] = get_shape(shape_id);
                if (shape != nullptr)
                {
                    shape->is_isolated = isolate;
                    m_render_data_by_mesh_by_type[mesh_id][type].needs_update = true;
                }
            }
        }
    }

    void ShapeRenderer::reset_visibility()
    {
        for (const auto& [cell_id, shapes_of_cell] : m_shapes_by_cell_id)
        {
            for (const auto& shape_id : shapes_of_cell)
            {
                auto [mesh_id, type, shape] = get_shape(shape_id);
                if (shape != nullptr)
                {
                    shape->is_dug = 0.0f;
                    shape->is_isolated = 0.0f;
                    m_render_data_by_mesh_by_type[mesh_id][type].needs_update = true;
                }
            }
        }
    }
}