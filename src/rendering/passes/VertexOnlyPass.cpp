#include "VertexOnlyPass.h"
#include "rendering/Renderer.h"

namespace volumeshOS::Internal
{
    VertexOnlyPass::VertexOnlyPass()
    {
        m_vertex_only_shader = Shader::vertex_only_shader();
    }

    void VertexOnlyPass::render(const Renderer& renderer)
    {
        // GL Setup
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        renderer.buffers.target_framebuffer_ms->bind();
        auto cam = renderer.camera;

        for (const auto& mesh: renderer.render_list)
        {
            const auto& data = renderer.pass_data_list.at(mesh->get_id());

            m_vertex_only_shader->bind();

            float size = AppState::settings.vertex_size;

            m_vertex_only_shader->set_uniform_mat4f("u_mesh_transform", data.transform);
            m_vertex_only_shader->set_uniform_mat4f("u_projection", cam->projection);
            m_vertex_only_shader->set_uniform_mat4f("u_view", cam->view);
            m_vertex_only_shader->set_uniform_vec3f("u_cam_pos", data.cam_pos);
            m_vertex_only_shader->set_uniform_int("u_selection_offset", mesh->get_data().selection_id_offset);
            m_vertex_only_shader->set_uniform_float("u_cell_size", mesh->get_data().cell_size);
            m_vertex_only_shader->set_uniform_int("u_peel_depth", (int) mesh->get_data().peel_level);
            m_vertex_only_shader->set_uniform_float("u_max_peel_depth", mesh->get_data().max_peel_depth);
            m_vertex_only_shader->set_uniform_bool("u_reverse_peeling", mesh->get_data().reverse_peeling);
            m_vertex_only_shader->set_uniform_float("u_slice_depth", mesh->get_data().slice_level);
            m_vertex_only_shader->set_uniform_vec3f("u_min", data.bb_min);
            m_vertex_only_shader->set_uniform_vec3f("u_max", data.bb_max);
            m_vertex_only_shader->set_uniform_vec3f("u_slice_direction", data.slice_direction);
            m_vertex_only_shader->set_uniform_bool("u_slice_locked", mesh->get_data().slice_locked);
            m_vertex_only_shader->set_uniform_float("u_average_cell_size", mesh->get_mvb()->get_average_cell_size());
            m_vertex_only_shader->set_uniform_vec4f("u_color", mesh->get_data().color);
            m_vertex_only_shader->set_uniform_float("u_size", size);

            mesh->get_mvb()->get_vertex_only_vao()->draw_instanced(mesh->get_num_visible_vertices());

            m_vertex_only_shader->unbind();
        }
        renderer.buffers.target_framebuffer_ms->unbind();
    }
}
