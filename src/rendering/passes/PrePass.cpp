
#include "PrePass.h"
#include "mesh/MeshProperties.h"
#include "../Renderer.h"

namespace volumeshOS::Internal
{
    void PrePass::draw_with_shader(const Renderer& renderer, Shader& shader)
    {
        shader.bind();

        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        auto pre_phong_shader = Shader::pre_mesh_phong_shader();

        pre_phong_shader->bind();

        for (const auto& mesh : renderer.render_list)
        {
            // We don't need to render the pre-pass if the mesh is set to points or lines
            if (mesh->get_data().rendering_mode != RenderingMode::CELLS)
            {
                continue;
            }

            bool is_bezier_mesh = mesh->is_bezier_mesh();
            bool use_vertex_normals = mesh->get_data().shading_mode == ShadingMode::PHONG;
            // Currently, cells sometimes appear hollow if CULL_FACE is not
            // disabled for Bézier meshes
            if ( is_bezier_mesh || mesh->get_data().use_two_sided_lighting)
            {
                glDisable(GL_CULL_FACE);
            }
            else
            {
                glEnable(GL_CULL_FACE);
                glFrontFace(GL_CCW);
                glCullFace(GL_BACK);
            }

            const auto& data = renderer.pass_data_list.at(mesh->get_id());


            // set all of our uniforms
            shader.set_uniform_mat4f("u_transform", data.transform);
            shader.set_uniform_mat4f("u_projection", data.projection);
            shader.set_uniform_mat4f("u_view", data.view);
            shader.set_uniform_vec3f("u_cam_pos", data.cam_pos);
            shader.set_uniform_vec3f("u_light_pos", data.light_pos);
            shader.set_uniform_float("u_cell_size", mesh->get_data().cell_size);
            shader.set_uniform_vec4f("u_object_color", mesh->get_data().color);
            shader.set_uniform_float("u_peel_depth", mesh->get_data().peel_level);
            shader.set_uniform_float("u_max_peel_depth", mesh->get_data().max_peel_depth);
            shader.set_uniform_bool("u_reverse_peeling", mesh->get_data().reverse_peeling);
            shader.set_uniform_float("u_slice_depth", mesh->get_data().slice_level);
            shader.set_uniform_vec3f("u_min", data.bb_min);
            shader.set_uniform_vec3f("u_max", data.bb_max);
            shader.set_uniform_vec3f("u_slice_direction", data.slice_direction);
            shader.set_uniform_bool("u_slice_locked", mesh->get_data().slice_locked);
            shader.set_uniform_bool("u_rounding", (is_bezier_mesh) ? false : mesh->get_data().rounding_size > 0.0f);
            shader.set_uniform_float("u_rounding_size", mesh->get_data().rounding_size);
            shader.set_uniform_float("u_average_cell_size", mesh->get_mvb()->get_average_cell_size());
            shader.set_uniform_bool("u_use_vertex_normals", use_vertex_normals);
            shader.set_uniform_bool("u_two_sided_lighting", mesh->get_data().use_two_sided_lighting);
            shader.set_uniform_bool("u_use_base_color", mesh->get_data().use_base_color);


            pre_phong_shader->set_uniform_bool("u_is_bezier_mesh", is_bezier_mesh);
            pre_phong_shader->set_uniform_bool("u_use_base_color", mesh->get_data().use_base_color);
            if(is_bezier_mesh)
            {
                auto mtb = mesh->get_mtb();
                // Use Bezier Mesh Property to set uniform.
                shader.set_uniform_int("u_bezier_degree", *mesh->get_ovm()->request_mesh_property<int>(MeshProperties::PROP_BEZIER_DEGREE).begin());

                // GL_TEXTURE12 is used for control points storage.
                shader.set_uniform_texbuffer("u_control_points_tb", mtb->get_binding(), mtb->get_texture());
                // Use tessellation level value from toolbar.
                shader.set_uniform_int("u_bezier_tessellation_level", mesh->get_data().tessellation_level);
            }

            auto vao = mesh->get_vao();
            if (mesh->get_data().rounding_size > 0.0f && !is_bezier_mesh)
            {
                vao = mesh->get_mvb()->get_vao_rounded();
            }

            vao->draw_patches();
        }
        shader.unbind();
    }

    void PrePass::render(const Renderer& renderer)
    {
        m_pre_pass_framebuffer->bind();

        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderer.passes.ground_pass->render_pre(renderer);

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
