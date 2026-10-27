#include "polyhydra/rendering/passes/TransparencyPassDP.h"

#include "polyhydra/mesh/MeshProperties.h"
#include "polyhydra/rendering/meshes/CommonMeshes.h"
#include "polyhydra/settings/AppState.h"

namespace polyhydra::Internal
{
class MeshView;

namespace
{
std::optional<FrameBufferAttachment> opaque_depth_attachment_specification()
{
    const std::vector<FrameBufferAttachment>& attachments = AppState::settings.multisampling
                                                                ? FrameBufferObject::RGBA_AND_DEPTH_MULTISAMPLE
                                                                : FrameBufferObject::RGBA_AND_DEPTH;
    for (const auto& attachment : attachments)
    {
        if (attachment.attachment == GL_DEPTH_ATTACHMENT || attachment.attachment == GL_DEPTH_STENCIL_ATTACHMENT)
        {
            return attachment;
        }
    }
    return std::nullopt;
}

FrameBufferAttachment single_sample_depth_attachment(const FrameBufferAttachment& specification)
{
    FrameBufferAttachment attachment = specification;
    attachment.multisample = false;
    return attachment;
}

void copy_depth_attachment(const FrameBufferObject& source, const FrameBufferObject& destination)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, source.get_id());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destination.get_id());
    glBlitFramebuffer(0,
                      0,
                      source.get_width(),
                      source.get_height(),
                      0,
                      0,
                      destination.get_width(),
                      destination.get_height(),
                      GL_DEPTH_BUFFER_BIT,
                      GL_NEAREST);
}

FrameBufferAttachment fallback_depth_attachment()
{
    return FrameBufferAttachment{.internal_format = GL_DEPTH_COMPONENT24,
                                 .format = GL_DEPTH_COMPONENT,
                                 .type = GL_UNSIGNED_INT,
                                 .attachment = GL_DEPTH_ATTACHMENT,
                                 .texture_filter = GL_NEAREST};
}
} // namespace

TransparencyPassDP::TransparencyPassDP(int width, int height) : m_width(width), m_height(height)
{
    m_transparency_shader = Shader::get("transparency_dp");
    m_composite_shader = Shader::get("composite_dp");
}

TransparencyPassDP::~TransparencyPassDP()
{
    release_render_target_resources();
}

void TransparencyPassDP::release_render_target_resources()
{
    m_transparent_framebuffer0.reset();
    m_transparent_framebuffer1.reset();
    m_opaque_depth_framebuffer.reset();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    if (!m_layer_textures.empty())
    {
        glDeleteTextures(static_cast<GLsizei>(m_layer_textures.size()), m_layer_textures.data());
        m_layer_textures.clear();
    }

    m_layer_width = 0;
    m_layer_height = 0;
    m_copy_opaque_depth = false;
}

void TransparencyPassDP::create_peel_framebuffers()
{
    const std::optional<FrameBufferAttachment> opaque_depth = opaque_depth_attachment_specification();
    m_copy_opaque_depth = opaque_depth.has_value();
    if (!m_copy_opaque_depth)
    {
        Log::warn("The target framebuffer has no depth attachment in one of the default formats, "
                  "transparent fragments are not hidden by opaque geometry");
    }
    const FrameBufferAttachment depth_attachment
        = single_sample_depth_attachment(opaque_depth.value_or(fallback_depth_attachment()));

    const std::vector<FrameBufferAttachment> attachments
        = {FrameBufferAttachment{.internal_format = GL_RGBA16F,
                                  .format = GL_RGBA,
                                  .type = GL_FLOAT,
                                  .attachment = GL_COLOR_ATTACHMENT0,
                                  .texture_filter = GL_LINEAR,
                                  .texture_wrap = GL_CLAMP_TO_EDGE},
            FrameBufferAttachment{.internal_format = GL_R32F,
                                  .format = GL_RED,
                                  .type = GL_FLOAT,
                                  .attachment = GL_COLOR_ATTACHMENT1,
                                  .texture_filter = GL_NEAREST,
                                  .texture_wrap = GL_CLAMP_TO_EDGE},
            depth_attachment};

    m_transparent_framebuffer0 = std::make_shared<FrameBufferObject>(m_width, m_height, attachments);
    m_transparent_framebuffer1 = std::make_shared<FrameBufferObject>(m_width, m_height, attachments);

    const std::vector<FrameBufferAttachment> opaque_depth_attachments{depth_attachment};
    m_opaque_depth_framebuffer = std::make_shared<FrameBufferObject>(m_width, m_height, opaque_depth_attachments);
}

void TransparencyPassDP::ensure_layer_textures(int num_passes)
{
    if (static_cast<int>(m_layer_textures.size()) == num_passes && m_layer_width == m_width
        && m_layer_height == m_height)
    {
        return;
    }

    // Recreate the whole pool instead of only growing it, so that its size follows the number of
    // peel passes and the resolution instead of only ever increasing.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    if (!m_layer_textures.empty())
    {
        glDeleteTextures(static_cast<GLsizei>(m_layer_textures.size()), m_layer_textures.data());
    }

    m_layer_textures.assign(static_cast<size_t>(num_passes), 0);
    glGenTextures(num_passes, m_layer_textures.data());

    for (const uint32_t texture : m_layer_textures)
    {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    m_layer_width = m_width;
    m_layer_height = m_height;
}

void TransparencyPassDP::render(const Renderer& renderer)
{
    auto& settings = AppState::settings;
    int num_passes = settings.num_depth_peeling_passes;
    if (num_passes <= 0)
    {
        return;
    }

    // Recreated at the first render() call and whenever the resources were released, e.g. by a
    // window resize (see resize_buffers).
    if (!m_transparent_framebuffer0 || !m_transparent_framebuffer1 || !m_opaque_depth_framebuffer)
    {
        release_render_target_resources();
        create_peel_framebuffers();
    }

    ensure_layer_textures(num_passes);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    // Clear value of the depth copy attachments (COLOR_ATTACHMENT1): a pixel that this peel pass
    // did not write has to read as "nothing peeled here yet", i.e. the far plane.
    const float far_plane[4] = {1.0f, 0.0f, 0.0f, 1.0f};

    // Collect the depth of the opaque scene once per frame, from where it is copied into the depth
    // buffer of every peel framebuffer (see create_peel_framebuffers).
    if (m_copy_opaque_depth)
    {
        copy_depth_attachment(*renderer.buffers.target_framebuffer_ms, *m_opaque_depth_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    for (int i = 0; i < num_passes; i++)
    {
        const std::shared_ptr<FrameBufferObject>& current
            = (i % 2 == 0) ? m_transparent_framebuffer0 : m_transparent_framebuffer1;
        const std::shared_ptr<FrameBufferObject>& previous
            = (i % 2 == 0) ? m_transparent_framebuffer1 : m_transparent_framebuffer0;

        current->bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_layer_textures[i], 0);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearBufferfv(GL_COLOR, 1, far_plane);

        if (m_copy_opaque_depth)
        {
            // Fragments that are hidden by opaque geometry never enter a peel layer, since the depth
            // buffer of this pass is initialized with the depth of the opaque scene. Afterwards the
            // read framebuffer is switched back, current stays bound for rendering.
            copy_depth_attachment(*m_opaque_depth_framebuffer, *current);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, current->get_id());
        }
        else
        {
            glClearDepth(1.0);
            glClear(GL_DEPTH_BUFFER_BIT);
        }

        m_transparency_shader->bind();

        // Depth of the fragments that all previous peel layers already contain, read from the float
        // depth copy attachment of the previous peel pass.
        const uint32_t last_depth_texture = previous->get_texture(GL_COLOR_ATTACHMENT1);
        m_transparency_shader->set_uniform_sampler2D("last_depth_texture", GL_TEXTURE2, last_depth_texture);

        for (const auto& mesh : renderer.render_list)
        {
            render_mesh(renderer, mesh, i);
        }
        m_transparency_shader->unbind();
    }

    // Composite the peeled layers from the farthest to the nearest one.
    for (int i = num_passes - 1; i >= 0; i--)
    {
        render_composition(renderer, i, num_passes, m_layer_textures[i]);
    }

    // None of the peel framebuffers must stay bound, their attachments are sampled above.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TransparencyPassDP::render_mesh(const Renderer& renderer, const std::shared_ptr<MeshObject>& mesh, int layer)
{
    // We don't need transparent points or lines
    if (!mesh->get_data().cells)
    {
        return;
    }

    if (!mesh->get_data().use_back_face_culling)
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
    m_transparency_shader->set_uniform_bool("u_rounding",
                                            (is_bezier_mesh) ? false : mesh->get_data().rounding_size > 0.0f);
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

    m_transparency_shader->set_uniform_bool("u_is_bezier_mesh", is_bezier_mesh);

    // The sampler for the Bezier control points is part of the tessellation evaluation shader of
    // every mesh and only skipped by a dynamic branch, so it always needs to reference a valid
    // texture buffer. An unset sampler uniform would default to texture unit 0.
    auto mtb = mesh->get_mtb();
    // GL_TEXTURE12 is used for control points storage.
    m_transparency_shader->set_uniform_texbuffer("u_control_points_tb", mtb->get_binding(), mtb->get_texture());

    if (is_bezier_mesh)
    {
        // Use Bezier Mesh Property to set uniform.
        m_transparency_shader->set_uniform_int(
            "u_bezier_degree",
            *mesh->get_ovm()->request_mesh_property<int>(MeshProperties::PROP_BEZIER_DEGREE).begin());

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

    // The peel framebuffers and layer textures are recreated at the next render() call.
    release_render_target_resources();
}

} // namespace polyhydra::Internal
