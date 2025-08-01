#include "TransparencyPassDP.h"
#include "../meshes/CommonMeshes.h"
#include "../../settings/AppState.h"
#include "mesh/MeshProperties.h"

namespace volumeshOS::Internal
{
    class MeshView;

    TransparencyPassDP::TransparencyPassDP(int width, int height) :
            m_width(width),
            m_height(height)
    {
        m_transparency_shader = Shader::get("transparency_dp");
        m_composite_shader = Shader::get("composite_dp");

        std::vector<FrameBufferAttachment> transparent_attachments0 =
                {
                        FrameBufferAttachment
                                {
                                        .internal_format    = GL_RGBA16F,
                                        .format             = GL_RGBA,
                                        .type               = GL_FLOAT,
                                        .attachment         = GL_COLOR_ATTACHMENT0,
                                        .texture_filter     = GL_LINEAR,
                                        .texture_wrap       = GL_CLAMP_TO_EDGE
                                },
                        FrameBufferAttachment
                                {
                                        .internal_format    = GL_DEPTH_COMPONENT32F,
                                        .format             = GL_DEPTH_COMPONENT,
                                        .type               = GL_FLOAT,
                                        .attachment         = GL_DEPTH_ATTACHMENT,
                                        .texture_filter     = GL_NEAREST
                                }
                };
        m_transparent_framebuffer0 = std::make_shared<FrameBufferObject>(width, height, transparent_attachments0);

        std::vector<FrameBufferAttachment> transparent_attachments1 =
                {
                        FrameBufferAttachment{
                                .internal_format    = GL_RGBA16F,
                                .format             = GL_RGBA,
                                .type               = GL_FLOAT,
                                .attachment         = GL_COLOR_ATTACHMENT0,
                                .texture_filter     = GL_LINEAR,
                                .texture_wrap       = GL_CLAMP_TO_EDGE

                        },
                        FrameBufferAttachment
                                {
                                        .internal_format    = GL_DEPTH_COMPONENT32F,
                                        .format             = GL_DEPTH_COMPONENT,
                                        .type               = GL_FLOAT,
                                        .attachment         = GL_DEPTH_ATTACHMENT,
                                        .texture_filter     = GL_NEAREST
                                }
                };
        m_transparent_framebuffer1 = std::make_shared<FrameBufferObject>(width, height, transparent_attachments1);
    }

    void TransparencyPassDP::render(const Renderer& renderer)
    {

        auto& settings = AppState::settings;
        int num_passes = settings.num_depth_peeling_passes;

        // TODO: Refactor this pass so that textures are not allocated every frame. Rather check if something changed
        //       and then update allocations
        std::vector<uint32_t> textures;
        for(int i = 0; i < num_passes; i++)
        {
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA,GL_FLOAT, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            textures.push_back(texture);
        }

        for (int i = 0; i < num_passes; i++)
        {
            if (i % 2 == 0)
            {
                m_transparent_framebuffer0->bind();

            }
            else
            {
                m_transparent_framebuffer1->bind();

            }
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[i], 0);

            if(i == 0)
            {
                m_transparent_framebuffer1->bind();
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                m_transparent_framebuffer1->unbind();
                m_transparent_framebuffer0->bind();
            }
            glClearDepth(1.0f);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);


            if (i % 2 == 0)
            {
                m_transparent_framebuffer0->bind();
                m_transparency_shader->bind();

                uint32_t depth_texture = m_transparent_framebuffer1->get_texture(GL_DEPTH_ATTACHMENT);
                m_transparency_shader->set_uniform_sampler2D("last_depth_texture", GL_TEXTURE2, depth_texture);
                for (const auto& mesh: renderer.render_list)
                {
                    render_mesh(renderer, mesh, i);
                }
                m_transparency_shader->unbind();
                m_transparent_framebuffer0->unbind();
            }
            else
            {
                m_transparent_framebuffer1->bind();
                m_transparency_shader->bind();
                uint32_t depth_texture = m_transparent_framebuffer0->get_texture(GL_DEPTH_ATTACHMENT);
                m_transparency_shader->set_uniform_sampler2D("last_depth_texture", GL_TEXTURE2, depth_texture);
                for (const auto& mesh: renderer.render_list)
                {
                    render_mesh(renderer, mesh, i);
                }
                m_transparency_shader->unbind();
                m_transparent_framebuffer1->unbind();
            }

        }
        for (int i = num_passes - 1; i >= 0; i--)
        {
            render_composition(renderer, i, num_passes, textures[i]);
        }

        glDeleteTextures((int) textures.size(), textures.data());
        textures.clear();
    }


    void TransparencyPassDP::render_mesh(const Renderer& renderer, const std::shared_ptr<MeshObject>& mesh, int layer)
    {
        // We don't need transparent points or lines
        if (!mesh->get_data().cells)
        {
            return;
        }

        if(!mesh->get_data().use_back_face_culling)
        {
            glDisable(GL_CULL_FACE);
        }

        const auto& data = renderer.pass_data_list.at(mesh->get_id());

        auto cam = renderer.camera;
        auto light = AppState::settings.light;


        bool use_vertex_normals = mesh->get_data().shading_mode == ShadingMode::PHONG;
        
        bool is_bezier_mesh = mesh->is_bezier_mesh();
        // Currently, cells sometimes appear hollow if CULL_FACE is not 
        // disabled for Bézier meshes
        if (is_bezier_mesh)
        {
            glDisable(GL_CULL_FACE);
        }

        // set all of our uniforms
        m_transparency_shader->set_uniform_mat4f("u_transform", data.transform);
        m_transparency_shader->set_uniform_mat4f("u_projection", cam->projection);
        m_transparency_shader->set_uniform_mat4f("u_view", cam->view);
        m_transparency_shader->set_uniform_vec3f("u_light_pos", data.light_pos);
        m_transparency_shader->set_uniform_vec3f("u_cam_pos", data.cam_pos);
        m_transparency_shader->set_uniform_vec3f("u_light_color", light.color);
        m_transparency_shader->set_uniform_float("u_cell_size", mesh->get_data().cell_size);
        m_transparency_shader->set_uniform_vec4f("u_object_color", mesh->get_data().color);
        m_transparency_shader->set_uniform_float("u_peel_depth", mesh->get_data().peel_level);
        m_transparency_shader->set_uniform_float("u_max_peel_depth", mesh->get_data().max_peel_depth);
        m_transparency_shader->set_uniform_bool("u_reverse_peeling", mesh->get_data().reverse_peeling);
        m_transparency_shader->set_uniform_float("u_slice_depth", mesh->get_data().slice_level);
        m_transparency_shader->set_uniform_vec3f("u_min", data.bb_min);
        m_transparency_shader->set_uniform_vec3f("u_max", data.bb_max);
        m_transparency_shader->set_uniform_vec3f("u_slice_direction", data.slice_direction);
        m_transparency_shader->set_uniform_bool("u_slice_locked", mesh->get_data().slice_locked);
        m_transparency_shader->set_uniform_float("u_pow", m_pow);
        m_transparency_shader->set_uniform_float("u_alpha_pow", m_alpha_pow);
        m_transparency_shader->set_uniform_float("u_range", m_range);
        m_transparency_shader->set_uniform_float("u_depth_range", m_depth_range);
        m_transparency_shader->set_uniform_float("u_ordering_strength", m_ordering_strength);
        m_transparency_shader->set_uniform_float("u_t_min", m_min);
        m_transparency_shader->set_uniform_float("u_t_max", m_max);
        // Do not use rounding on Bézier meshes.
        m_transparency_shader->set_uniform_bool("u_rounding", (is_bezier_mesh) ? false : mesh->get_data().rounding_size > 0.0f);
        m_transparency_shader->set_uniform_float("u_rounding_size", mesh->get_data().rounding_size);
        m_transparency_shader->set_uniform_float("u_average_cell_size", mesh->get_mvb()->get_average_cell_size());
        m_transparency_shader->set_uniform_int("u_viewport_width", renderer.buffers.target_framebuffer->get_width());
        m_transparency_shader->set_uniform_int("u_viewport_height", renderer.buffers.target_framebuffer->get_height());
        m_transparency_shader->set_uniform_float("u_spec_strength", mesh->get_data().specular_strength);
        m_transparency_shader->set_uniform_float("u_spec_exponent", mesh->get_data().specular_exponent);
        m_transparency_shader->set_uniform_float("u_ambient_strength", mesh->get_data().ambient_strength);
        m_transparency_shader->set_uniform_float("u_diffuse_strength", mesh->get_data().diffuse_strength);
        m_transparency_shader->set_uniform_bool("u_use_vertex_normals", use_vertex_normals);
        m_transparency_shader->set_uniform_int("u_current_layer", layer);

        m_transparency_shader->set_uniform_bool("u_draw_lines", mesh->get_data().lines);
        m_transparency_shader->set_uniform_bool("u_use_base_color", mesh->get_data().use_base_color);
        m_transparency_shader->set_uniform_bool("u_two_sided_lighting", mesh->get_data().use_two_sided_lighting);

        m_transparency_shader->set_uniform_bool("u_use_pbr", mesh->get_data().use_pbr);
        m_transparency_shader->set_uniform_float("u_metallic", mesh->get_data().metallic);
        m_transparency_shader->set_uniform_float("u_roughness", mesh->get_data().roughness);
        m_transparency_shader->set_uniform_float("u_light_intensity", light.intensity);
        m_transparency_shader->set_uniform_float("u_gamma", AppState::settings.post_processing.gamma);
        m_transparency_shader->set_uniform_vec3f("u_ground_color", AppState::settings.ground.solid_color);
        m_transparency_shader->set_uniform_vec3f("u_background_color", AppState::settings.sky.sky_color);

        uint32_t depth_texture = renderer.buffers.target_framebuffer->get_texture(GL_DEPTH_ATTACHMENT);
        m_transparency_shader->set_uniform_sampler2D("max_depth_texture", GL_TEXTURE1, depth_texture);

        m_transparency_shader->set_uniform_bool("u_is_bezier_mesh", is_bezier_mesh);
        if(is_bezier_mesh)
        {
            auto mtb = mesh->get_mtb();
            // Use Bezier Mesh Property to set uniform.
            m_transparency_shader->set_uniform_int("u_bezier_degree", *mesh->get_ovm()->request_mesh_property<int>(MeshProperties::PROP_BEZIER_DEGREE).begin());

            // GL_TEXTURE12 is used for control points storage.
            m_transparency_shader->set_uniform_texbuffer("u_control_points_tb", mtb->get_binding(), mtb->get_texture());
            // Use tessellation level value from toolbar.
            m_transparency_shader->set_uniform_int("u_bezier_tessellation_level", mesh->get_data().tessellation_level);
        }

        auto vao = mesh->get_vao();
        if (mesh->get_data().rounding_size > 0.0f && !is_bezier_mesh)
        {
            vao = mesh->get_mvb()->get_vao_rounded();
        }
        vao->draw_patches();

    }

    void TransparencyPassDP::render_composition(const Renderer& renderer, int current_pass, int max_passes, uint32_t tex)
    {

        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);

        if (renderer.frame.is_rendering_background)
        {
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
        }
        else
        {
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        }

        uint32_t new_layer;

        if (current_pass % 2 == 0)
        {
            new_layer = m_transparent_framebuffer0->get_texture(GL_COLOR_ATTACHMENT0);
        }
        else
        {
            new_layer = m_transparent_framebuffer1->get_texture(GL_COLOR_ATTACHMENT0);
        }


        renderer.buffers.target_framebuffer_ms->bind();
        m_composite_shader->bind();
        m_composite_shader->set_uniform_int("u_current_pass", current_pass);
        m_composite_shader->set_uniform_int("u_max_passes", max_passes - 1);

        m_composite_shader->set_uniform_sampler2D("new_layer_texture", GL_TEXTURE0, tex);
        VertexArrayObject::draw_screen_quad();
        m_composite_shader->unbind();
        renderer.buffers.target_framebuffer_ms->unbind();


        glDisable(GL_BLEND);
        glClearDepth(1.0f);
        glEnable(GL_CULL_FACE);

    }

    void TransparencyPassDP::resize_buffers(int width, int height)
    {
        m_width = width;
        m_height = height;
        m_transparent_framebuffer0->resize(width, height);
        m_transparent_framebuffer1->resize(width, height);
    }

    void TransparencyPassDP::update_draw_texture()
    {
        //uint32_t texture = renderer.buffers.target_framebuffer_ms->get_texture(GL_COLOR_ATTACHMENT0);
//        m_transparent_framebuffer0->bind();
//        m_transparent_framebuffer0->attach_texture(GL_COLOR_ATTACHMENT0, texture);
//        m_transparent_framebuffer0->unbind();
//        m_transparent_framebuffer1->bind();
//        m_transparent_framebuffer1->attach_texture(GL_COLOR_ATTACHMENT0, texture);
//        m_transparent_framebuffer1->unbind();
    }



}
