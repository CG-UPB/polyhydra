
#include "VertexOnlyPass.h"

#include "../../mesh/MeshObject.h"
#include "panels/Window.h"

namespace volumeshOS::Internal
{
    VertexOnlyPass::VertexOnlyPass()
    {
        m_vertex_only_shader = Shader::vertex_only_shader();
    }

    void VertexOnlyPass::render(std::shared_ptr<VertexArrayObject> vao, const RenderData& data, std::shared_ptr<MeshObject> mesh)
    {
        // GL Setup
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        // Transform Data
        glm::mat4 transform = data.camera.world * mesh->get_data().get_transform();
        glm::mat4 view_transform = data.camera.view * transform;

        // Cell operations
        float cell_size = mesh->get_data().cell_size;
        float peel_depth = mesh->get_data().peel_level;
        float slice_depth = mesh->get_data().slice_level;

        auto bb = mesh->get_world_bb(view_transform);
        auto min = bb.first;
        auto max = bb.second;

        // volumeshOS Operations
        glm::vec3 view_dir = -glm::normalize(data.camera.get_front());
        auto slice_direction = mesh->get_slice_dir(view_transform, view_dir);

        glm::vec3 cam_pos(data.camera.view * glm::vec4(data.camera.position, 1.0));

        m_vertex_only_shader->bind();

        float size = GlobalViewerSettings::getInstance()->get_vertex_only_size();

        m_vertex_only_shader->set_uniform_mat4f("u_mesh_transform", transform);
        m_vertex_only_shader->set_uniform_mat4f("u_projection", data.camera.projection);
        m_vertex_only_shader->set_uniform_mat4f("u_view", data.camera.view);
        m_vertex_only_shader->set_uniform_vec3f("u_cam_pos", cam_pos);
        m_vertex_only_shader->set_uniform_int("u_selection_offset", mesh->get_data().selection_id_offset);
        m_vertex_only_shader->set_uniform_float("u_cell_size", cell_size);
        m_vertex_only_shader->set_uniform_int("u_peel_depth", (int) peel_depth);
        m_vertex_only_shader->set_uniform_float("u_slice_depth", slice_depth);
        m_vertex_only_shader->set_uniform_vec3f("u_min", min);
        m_vertex_only_shader->set_uniform_vec3f("u_max", max);
        m_vertex_only_shader->set_uniform_vec3f("u_slice_direction", slice_direction);
        m_vertex_only_shader->set_uniform_bool("u_slice_locked", mesh->get_data().slice_locked);
        m_vertex_only_shader->set_uniform_float("u_average_cell_size", mesh->get_mvb()->get_average_cell_size());
        m_vertex_only_shader->set_uniform_vec4f("u_color", mesh->get_data().color.get_rgba());
        m_vertex_only_shader->set_uniform_float("u_size", size);

        mesh->get_mvb()->get_vertex_only_vao()->draw_instanced(mesh->get_num_visible_vertices());

        m_vertex_only_shader->unbind();
    }
}
