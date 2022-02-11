
#include <iostream>
#include "glad/glad.h"
#include "../../Window.h"
#include "TransparencyPass.h"
#include "../../settings/GlobalViewerSettings.h"
#include "../meshes/CommonMeshes.h"


namespace vOS
{
    class MeshView;

    TransparencyPass::TransparencyPass(MeshView *mesh_view, unsigned int width, unsigned int height):
    m_mesh_view(mesh_view),
    m_width(width),
    m_height(height)
    {
        m_transparency_shader = Shader::get("transparency_wb");
        m_composite_shader = Shader::get("composite");

        m_vao = new VertexArrayObject(CommonMeshes::PlaneXY::vertices(2.0f, 2.0f), CommonMeshes::PlaneXY::indices());
        m_vao->add_attribute(CommonMeshes::PlaneXY::uvs(), 1, 2);

        generate_transparency_framebuffer(m_width, m_height);

    }

    TransparencyPass::~TransparencyPass()
    {
        clean_up_framebuffer();
        glDeleteTextures(1, &m_accumTexture);
        glDeleteTextures(1, &m_revealTexture);
        delete m_vao;

    }

    void TransparencyPass::generate_transparency_framebuffer(unsigned int width, unsigned int height)
    {
        m_width = width;
        m_height = height;
        glGenFramebuffers(1, &m_transparent_framebuffer);

        glGenTextures(1, &m_accumTexture);
        glBindTexture(GL_TEXTURE_2D, m_accumTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, (int)m_width, (int)m_height, 0, GL_RGBA, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenTextures(1, &m_revealTexture);
        glBindTexture(GL_TEXTURE_2D, m_revealTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, (int)m_width, (int)m_height, 0, GL_RED, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
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
            std::cout << "ERROR::FRAMEBUFFER:: Transparent Framebuffer is not complete! " << glCheckFramebufferStatus(GL_FRAMEBUFFER)<< std::endl;
        }
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
        {
            std::cout << "ERROR::FRAMEBUFFER:: INCOMPLETE ATTATCHMENT!" << std::endl;
        }



        glClearBufferfv(GL_COLOR, 0, &m_zeros[0]);
        glClearBufferfv(GL_COLOR, 1, &m_ones[0]);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

    void TransparencyPass::clean_up_framebuffer()
    {
        glDeleteFramebuffers(1, &m_transparent_framebuffer);
        glDeleteTextures(1, &m_accumTexture);
        glDeleteTextures(1, &m_revealTexture);

    }


    void TransparencyPass::render(VertexArrayObject* vao, const RenderData& data, int mesh_id)
    {
        // Get Mesh
        MeshObject* obj = Window::instance().get_mesh_obj(mesh_id);
        if(obj == nullptr)
            return;

        // remember previous framebuffer, so we can rebind it when we are done
//        int previous_framebuffer;
//        int previous_viewport[4];
//        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previous_framebuffer);
//        glGetIntegerv(GL_VIEWPORT, previous_viewport);

        //resize_buffers(previous_viewport[2], previous_viewport[3]);

        //glClear(GL_COLOR_BUFFER_BIT);

        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunci(0, GL_ONE, GL_ONE);
        glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
        glBlendEquation(GL_FUNC_ADD);


        //glBindFramebuffer(GL_FRAMEBUFFER, m_transparent_framebuffer);
        auto transparency_shader = Shader::get("transparency_wb");
        transparency_shader->bind();

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
        transparency_shader->set_uniform_mat4f("u_Transform", transform);
        transparency_shader->set_uniform_mat4f("u_Projection", data.camera.projection);
        transparency_shader->set_uniform_mat4f("u_View", data.camera.view);
        transparency_shader->set_uniform_vec3f("u_lightPos", data.light.position);
        transparency_shader->set_uniform_vec3f("u_camPos", data.camera.position);
        transparency_shader->set_uniform_vec3f("u_lightColor", data.light.color);
        transparency_shader->set_uniform_float("u_cell_size", cell_size);
        transparency_shader->set_uniform_vec4f("u_object_color", obj->get_data().m_color.get_rgba());
        transparency_shader->set_uniform_int("u_peel_depth", peel_depth);
        transparency_shader->set_uniform_float("u_slice_depth", slice_depth);
        transparency_shader->set_uniform_vec3f("u_min", min);
        transparency_shader->set_uniform_vec3f("u_max", max);
        transparency_shader->set_uniform_vec3f("u_slice_direction", slice_direction);
        transparency_shader->set_uniform_bool("u_slice_locked", obj->get_data().m_slice_locked);

        vao->draw();
        transparency_shader->unbind();

    }

    void TransparencyPass::render_composition()
    {

        m_composite_shader->bind();

//        m_composite_shader->set_uniform_sampler2D("accumTexture", GL_TEXTURE0, m_accumTexture);
//        m_composite_shader->set_uniform_sampler2D("revealTexture", GL_TEXTURE1, m_revealTexture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_accumTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_revealTexture);

        m_vao->draw();
        m_composite_shader->unbind();

    }

    void TransparencyPass::resize_buffers(unsigned int width, unsigned int height)
    {
        clean_up_framebuffer();
        generate_transparency_framebuffer(width, height);

    }

    void TransparencyPass::clear_framebuffer() const
    {
//        glBindFramebuffer(GL_FRAMEBUFFER, m_transparent_framebuffer);
//        //glClear(GL_COLOR_BUFFER_BIT);
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

}