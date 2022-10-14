
#include "PrePass.h"
#include "../Renderer.h"

namespace volumeshOS::Internal
{
    void PrePass::draw_with_shader(const Renderer& renderer, Shader& shader)
    {
        shader.bind();

        for (const auto& mesh : renderer.render_list)
        {
            if(mesh->get_data().use_two_sided_lighting)
            {
                glDisable(GL_CULL_FACE);
            }
            else
            {
                glEnable(GL_CULL_FACE);
            }

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
            shader.set_uniform_mat4f("u_transform", transform);
            shader.set_uniform_mat4f("u_projection", renderer.camera->projection);
            shader.set_uniform_mat4f("u_view", renderer.camera->view);
            shader.set_uniform_vec3f("u_cam_pos", cam_pos);
            shader.set_uniform_float("u_cell_size", cell_size);
            shader.set_uniform_vec4f("u_object_color", mesh->get_data().color);
            shader.set_uniform_float("u_peel_depth", peel_depth);
            shader.set_uniform_float("u_max_peel_depth", mesh->get_data().max_peel_depth);
            shader.set_uniform_bool("u_reverse_peeling", mesh->get_data().reverse_peeling);
            shader.set_uniform_float("u_slice_depth", slice_depth);
            shader.set_uniform_vec3f("u_min", min);
            shader.set_uniform_vec3f("u_max", max);
            shader.set_uniform_vec3f("u_slice_direction", slice_direction);
            shader.set_uniform_bool("u_slice_locked", mesh->get_data().slice_locked);
            shader.set_uniform_bool("u_rounding", mesh->get_data().rounding_active);
            shader.set_uniform_float("u_rounding_size", mesh->get_data().rounding_size);
            shader.set_uniform_float("u_average_cell_size", mesh->get_mvb()->get_average_cell_size());

            auto vao = mesh->get_vao();
            if (mesh->get_data().rounding_active)
            {
                vao = mesh->get_mvb()->get_vao_rounded();
            }

            vao->draw();
        }
        shader.unbind();
    }

    void PrePass::render(const Renderer& renderer)
    {
        m_pre_pass_framebuffer->bind();

        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderer.passes.ground_pass->render_pre(renderer);

        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        draw_with_shader(renderer, *Shader::pre_mesh_phong_shader());
        m_pre_pass_framebuffer->unbind();
    }

    PrePass::PrePass(int width, int height)
    {
        m_pre_pass_framebuffer = std::make_shared<PrePassFrameBufferObject>(width, height);
    }

    void PrePass::resize_buffers(int width, int height)
    {
        m_pre_pass_framebuffer->resize(width, height);
    }

    std::shared_ptr<PrePassFrameBufferObject> PrePass::get_framebuffer() const
    {
        return m_pre_pass_framebuffer;
    }
}