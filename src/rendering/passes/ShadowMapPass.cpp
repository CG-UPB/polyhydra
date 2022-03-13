
#include "glad/glad.h"
#include "../../Window.h"
#include "ShadowMapPass.h"

namespace vOS
{
    ShadowMapPass::ShadowMapPass(MeshView* mesh_view, int width, int height): m_mesh_view(mesh_view)
    {
        m_shadow_shader = Shader::get("shadow_map");

        std::vector<FrameBufferAttachment> attachments =
        {
                FrameBufferAttachment
                {
                        .internal_format    = GL_DEPTH_COMPONENT,
                        .format             = GL_DEPTH_COMPONENT,
                        .type               = GL_FLOAT,
                        .attachment         = GL_DEPTH_ATTACHMENT,
                        .texture_filter     = GL_NEAREST,
                        .texture_wrap       = GL_CLAMP_TO_EDGE,
                        .texture_comp_func  = GL_LEQUAL,
                        .texture_comp_mode  = GL_NONE,
                }
        };
        m_shadow_framebuffer                = new FrameBufferObject(width, height, attachments);

    }

    ShadowMapPass::~ShadowMapPass()
    {
        delete m_shadow_framebuffer;
    }

    void ShadowMapPass::render(VertexArrayObject* vao, const RenderData& data, int mesh_id)
    {
        // Get Mesh
        MeshObject* obj = Window::instance().get_mesh_obj(mesh_id);
        if(obj == nullptr)
            return;
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);

        m_shadow_framebuffer->bind();
        m_shadow_shader->bind();


        // Transform
        glm::mat4 light_projection = data.light.projection;
        glm::mat4 light_view = data.light.view;
        glm::mat4 transform = data.camera.world * obj->get_data().get_transform();
        glm::mat4 l_transform = data.light.world * obj->get_data().get_transform();
        glm::mat4 view_transform = data.camera.view * transform;

        // Cell operations
        float cell_size = obj->get_data().m_cell_size;
        int peel_depth = obj->get_data().m_peel_level;
        float slice_depth = obj->get_data().m_slice_level;

        auto bb = obj->get_transformed_bb(view_transform);
        auto min = bb.first;
        auto max = bb.second;

        // View Operations
        glm::vec3 view_dir = -glm::normalize(data.camera.get_front());
        auto slice_direction = obj->get_slice_dir(view_transform, view_dir);

        auto settings = GlobalViewerSettings::getInstance();

        // Shader uniforms
        m_shadow_shader->set_uniform_float("u_cell_size", cell_size);
        m_shadow_shader->set_uniform_int("u_peel_depth", peel_depth);
        m_shadow_shader->set_uniform_float("u_slice_depth", slice_depth);
        m_shadow_shader->set_uniform_vec3f("u_min", min);
        m_shadow_shader->set_uniform_vec3f("u_max", max);
        m_shadow_shader->set_uniform_vec3f("u_slice_direction", slice_direction);
        m_shadow_shader->set_uniform_bool("u_slice_locked", obj->get_data().m_slice_locked);
        m_shadow_shader->set_uniform_bool("u_draw_wireframe", settings->get_mesh_mode() == Wireframe);
        m_shadow_shader->set_uniform_bool("u_rounding", data.rounding.active);
        m_shadow_shader->set_uniform_float("u_rounding_size", data.rounding.size);

        m_shadow_shader->set_uniform_mat4f("u_light_projection", light_projection);
        m_shadow_shader->set_uniform_mat4f("u_light_view", light_view);
        m_shadow_shader->set_uniform_mat4f("u_transform", l_transform);

        m_shadow_shader->set_uniform_int("u_viewport_width", m_mesh_view->m_viewportPanelWidth);
        m_shadow_shader->set_uniform_int("u_viewport_height", m_mesh_view->m_viewportPanelHeight);

        vao->draw();

        m_shadow_shader->unbind();
        m_shadow_framebuffer->unbind();

        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);
    }

    void ShadowMapPass::resize_buffers(int width, int height)
    {
        m_shadow_framebuffer->resize(width, height);
    }

    FrameBufferObject* ShadowMapPass::get_framebuffer() const
    {
        return m_shadow_framebuffer;
    }

    unsigned int ShadowMapPass::get_shadow_map() const
    {
        return m_shadow_framebuffer->get_texture(GL_DEPTH_ATTACHMENT);
    }

}