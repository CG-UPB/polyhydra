
#include "OutlinePass.h"
#include "../gl/Shader.h"
#include "../Renderer.h"


namespace volumeshOS::Internal
{
    OutlinePass::OutlinePass()
    {
        m_shader = Shader::get("outline");
    }

    void OutlinePass::render(const Renderer& renderer)
    {
        renderer.buffers.target_framebuffer_ms->bind();
        renderer.passes.ground_pass->render_pre(renderer);

        m_shader->bind();

        for (const auto& mesh : renderer.render_list)
        {
            glm::mat4 transform = renderer.camera->world * mesh->get_data().get_transform();
            glm::mat4 view_transform = renderer.camera->view * transform;

            // Cell operations
            float cell_size = mesh->get_data().cell_size;
            float peel_depth = mesh->get_data().peel_level;
            float slice_depth = mesh->get_data().slice_level;

            auto bb = mesh->get_world_bb(view_transform);
            auto min = bb.first;
            auto max = bb.second;

            // volumeshOS Operations
            glm::vec3 view_dir = -glm::normalize(renderer.camera->get_front());
            auto slice_direction = mesh->get_slice_dir(transform, view_dir);

            glm::vec3 cam_pos(renderer.camera->view * glm::vec4(renderer.camera->position, 1.0));

            // set all of our uniforms
            m_shader->set_uniform_mat4f("u_transform", transform);
            m_shader->set_uniform_mat4f("u_projection", renderer.camera->projection);
            m_shader->set_uniform_mat4f("u_view", renderer.camera->view);
            m_shader->set_uniform_vec3f("u_cam_pos", cam_pos);
            m_shader->set_uniform_float("u_cell_size", cell_size);
            m_shader->set_uniform_vec4f("u_object_color", mesh->get_data().color);
            m_shader->set_uniform_float("u_peel_depth", peel_depth);
            m_shader->set_uniform_float("u_max_peel_depth", mesh->get_data().max_peel_depth);
            m_shader->set_uniform_bool("u_reverse_peeling", mesh->get_data().reverse_peeling);
            m_shader->set_uniform_float("u_slice_depth", slice_depth);
            m_shader->set_uniform_vec3f("u_min", min);
            m_shader->set_uniform_vec3f("u_max", max);
            m_shader->set_uniform_vec3f("u_slice_direction", slice_direction);
            m_shader->set_uniform_bool("u_slice_locked", mesh->get_data().slice_locked);
            m_shader->set_uniform_bool("u_rounding", mesh->get_data().rounding_active);
            m_shader->set_uniform_float("u_rounding_size", mesh->get_data().rounding_size);
            m_shader->set_uniform_float("u_average_cell_size", mesh->get_mvb()->get_average_cell_size());

            m_shader->set_uniform_float("u_outline_width", AppState::settings.outline.width);
            m_shader->set_uniform_int("u_viewport_width", renderer.frame.width);
            m_shader->set_uniform_int("u_viewport_height", renderer.frame.height);
            m_shader->set_uniform_vec4f("u_outline_color", AppState::settings.outline.color);

            auto vao = mesh->get_vao();
            if (mesh->get_data().rounding_active)
            {
                vao = mesh->get_mvb()->get_vao_rounded();
            }

            // first draw, only update the stencil buffer with the whole cell
            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CCW);
            glCullFace(GL_BACK);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);

            // disable color writes
            //glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            glEnable(GL_STENCIL_TEST);
            glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF);
            m_shader->set_uniform_bool("u_draw_outline", false);
            vao->draw();

            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00);
            glEnable(GL_BLEND);
            m_shader->set_uniform_bool("u_draw_outline", true);
            vao->draw();
            glDisable(GL_BLEND);
            glDisable(GL_STENCIL_TEST);
            glEnable(GL_DEPTH_TEST);
        }
        m_shader->unbind();


        renderer.buffers.target_framebuffer_ms->unbind();
    }
}