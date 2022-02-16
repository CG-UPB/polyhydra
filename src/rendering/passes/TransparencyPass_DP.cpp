
#include <iostream>
#include "glad/glad.h"
#include "../../Window.h"
#include "TransparencyPass_DP.h"
#include "../../settings/GlobalViewerSettings.h"
#include "../meshes/CommonMeshes.h"


namespace vOS
{
    class MeshView;

    TransparencyPass_DP::TransparencyPass_DP(MeshView *mesh_view, unsigned int width, unsigned int height):
    m_mesh_view(mesh_view),
    m_width(width),
    m_height(height)
    {
        m_transparency_shader = Shader::get("transparency_dp");

    }

    TransparencyPass_DP::~TransparencyPass_DP()
    {
        clean_up_framebuffer();
        glDeleteTextures(1, &m_accumTexture);
        glDeleteTextures(1, &m_revealTexture);
        delete m_vao;

    }

    void TransparencyPass_DP::generate_transparency_framebuffer(unsigned int width, unsigned int height)
    {
        m_width = width;
        m_height = height;
        glGenFramebuffers(1, &m_transparent_framebuffer);

        glGenTextures(1, &m_accumTexture);
        glBindTexture(GL_TEXTURE_2D, m_accumTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, (int)m_width, (int)m_height, 0, GL_RGBA, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenTextures(1, &m_revealTexture);
        glBindTexture(GL_TEXTURE_2D, m_revealTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, (int)m_width, (int)m_height, 0, GL_RED, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, m_transparent_framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_accumTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_revealTexture, 0);
//        unsigned int text = m_mesh_view->m_meshFrameBuffer->get_texture_id();
//        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, text, 0);
        unsigned int opaque_depth = m_mesh_view->m_pre_pass_framebuffer->get_depth_texture_id();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, opaque_depth, 0);

        const GLenum transparentDrawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, transparentDrawBuffers);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "ERROR::FRAMEBUFFER:: Transparent Framebuffer is not complete! " << glCheckFramebufferStatus(GL_FRAMEBUFFER)<< std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

    void TransparencyPass_DP::clean_up_framebuffer()
    {
        glDeleteFramebuffers(1, &m_transparent_framebuffer);
        glDeleteTextures(1, &m_accumTexture);
        glDeleteTextures(1, &m_revealTexture);

    }


    void TransparencyPass_DP::render(VertexArrayObject* vao, const RenderData& data, int mesh_id)
    {
        // Get Mesh
        MeshObject* obj = Window::instance().get_mesh_obj(mesh_id);
        if(obj == nullptr)
            return;

        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);


        glBindFramebuffer(GL_FRAMEBUFFER, m_transparent_framebuffer);
        m_transparency_shader->bind();

        glm::mat4 positionOffset = glm::translate(-obj->get_data().offset);
        glm::mat4 transform = data.camera.world * obj->get_data().get_transform() * positionOffset;
        float cell_size = obj->get_data().m_cell_size;
        int peel_depth = obj->get_data().m_peel_level;
        float slice_depth = obj->get_data().m_slice_level;

        auto bb = obj->get_transformed_bb(transform);
        auto min = bb.first;
        auto max = bb.second;

        glm::mat4 view_inv = glm::inverse(data.camera.view);
        glm::vec3 view_dir = {view_inv[2][0], view_inv[2][1], view_inv[2][2]};
        auto slice_direction = obj->get_slice_dir(transform, view_dir);

        // set all of our uniforms
        m_transparency_shader->set_uniform_mat4f("u_Transform", transform);
        m_transparency_shader->set_uniform_mat4f("u_Projection", data.camera.projection);
        m_transparency_shader->set_uniform_mat4f("u_View", data.camera.view);
        m_transparency_shader->set_uniform_vec3f("u_lightPos", data.light.position);
        m_transparency_shader->set_uniform_vec3f("u_camPos", data.camera.position);
        m_transparency_shader->set_uniform_vec3f("u_lightColor", data.light.color);
        m_transparency_shader->set_uniform_float("u_cell_size", cell_size);
        m_transparency_shader->set_uniform_vec4f("u_object_color", obj->get_data().m_color.get_rgba());
        m_transparency_shader->set_uniform_int("u_peel_depth", peel_depth);
        m_transparency_shader->set_uniform_float("u_slice_depth", slice_depth);
        m_transparency_shader->set_uniform_vec3f("u_min", min);
        m_transparency_shader->set_uniform_vec3f("u_max", max);
        m_transparency_shader->set_uniform_vec3f("u_slice_direction", slice_direction);
        m_transparency_shader->set_uniform_bool("u_slice_locked", obj->get_data().m_slice_locked);

        m_transparency_shader->set_uniform_int("u_viewport_width", m_mesh_view->m_screen_quad_frameBuffer->get_width());
        m_transparency_shader->set_uniform_int("u_viewport_height", m_mesh_view->m_screen_quad_frameBuffer->get_height());

        vao->draw();
        m_transparency_shader->unbind();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

    void TransparencyPass_DP::resize_buffers(unsigned int width, unsigned int height)
    {
        clean_up_framebuffer();
        generate_transparency_framebuffer(width, height);

    }


}