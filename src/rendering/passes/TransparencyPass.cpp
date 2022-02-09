
#include <iostream>
#include "glad/glad.h"
#include "../../Window.h"
#include "TransparencyPass.h"
#include "../../settings/GlobalViewerSettings.h"

namespace vOS
{
    class MeshView;

    TransparencyPass::TransparencyPass(MeshView *mesh_view, unsigned int width, unsigned int height):
    m_mesh_view(mesh_view),
    m_width(width),
    m_height(height)
    {
        m_transparency_shader = Shader::get("transparency_WB");
        m_composite_shader = Shader::get("composite");
        generate_transparency_framebuffer();

    }

    TransparencyPass::~TransparencyPass()
    {
        clean_up_framebuffer();
        glDeleteTextures(1, &m_accumTexture);
        glDeleteTextures(1, &m_revealTexture);

    }

    void TransparencyPass::generate_transparency_framebuffer()
    {
        glGenFramebuffers(1, &m_transparent_framebuffer);

        glGenTextures(1, &m_accumTexture);
        glBindTexture(GL_TEXTURE_2D, m_accumTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, (int)m_width, (int)m_height, 0, GL_RGBA, GL_HALF_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenTextures(1, &m_revealTexture);
        glBindTexture(GL_TEXTURE_2D, m_revealTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, (int)m_width, (int)m_height, 0, GL_RED, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, m_transparent_framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_accumTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_revealTexture, 0);
        unsigned int opaque_depth = m_mesh_view->m_meshFrameBuffer->get_depth_texture_id();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, opaque_depth, 0);

        const GLenum transparentDrawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, transparentDrawBuffers);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "ERROR::FRAMEBUFFER:: Transparent Framebuffer is not complete!" << std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

    void TransparencyPass::clean_up_framebuffer()
    {

    }


    void TransparencyPass::render(VertexArrayObject* vao, const RenderData& data, int mesh_id)
    {
        // Get Mesh
        MeshObject* obj = Window::instance().get_mesh_obj(mesh_id);
        if(obj == nullptr)
            return;

        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunci(0, GL_ONE, GL_ONE);
        glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
        glBlendEquation(GL_FUNC_ADD);

        glm::vec4 zeros(0.0f);
        glm::vec4 ones(1.0f);

        glBindFramebuffer(GL_FRAMEBUFFER, m_transparent_framebuffer);
        glClearBufferfv(GL_COLOR, 0, &zeros[0]);
        glClearBufferfv(GL_COLOR, 1, &ones[0]);

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
        m_transparency_shader->set_uniform_vec4f("u_objectColor", obj->get_data().m_color.get_rgba());
        m_transparency_shader->set_uniform_int("u_peel_depth", peel_depth);
        m_transparency_shader->set_uniform_float("u_slice_depth", slice_depth);
        m_transparency_shader->set_uniform_vec3f("u_min", min);
        m_transparency_shader->set_uniform_vec3f("u_max", max);
        m_transparency_shader->set_uniform_vec3f("u_slice_direction", slice_direction);
        m_transparency_shader->set_uniform_bool("u_slice_locked", obj->get_data().m_slice_locked);

        vao->draw();
        m_transparency_shader->unbind();


    }

    void TransparencyPass::renderComposition()
    {
        glDepthFunc(GL_ALWAYS);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        m_mesh_view->m_meshFrameBuffer->bind();
        m_composite_shader->bind();

        m_composite_shader->set_uniform_sampler2D("accumTexture", 0, m_accumTexture);
        m_composite_shader->set_uniform_sampler2D("revealTexture", 1, m_revealTexture);


        m_composite_shader->unbind();
        m_mesh_view->m_meshFrameBuffer->unbind();

        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


}