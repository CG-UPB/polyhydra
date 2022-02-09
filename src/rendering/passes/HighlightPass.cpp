
#include "glad/glad.h"

#include "HighlightPass.h"
#include "../meshes/CommonMeshes.h"
#include "../../Window.h"

#include "glm/gtx/transform.hpp"

namespace vOS
{
    HighlightPass::HighlightPass()
    {
        m_vao = new VertexArrayObject(CommonMeshes::PlaneXY::vertices(), CommonMeshes::PlaneXY::indices());
        m_vao->add_attribute(CommonMeshes::PlaneXY::uvs(), 1, 2);
        m_highlight_shader = Shader::quad_circle_shader();
    }

    HighlightPass::~HighlightPass()
    {
        delete m_vao;
    }

    void HighlightPass::render(VertexArrayObject* vao, const RenderData& data, int mesh_id) {
        // Get Mesh
        MeshObject *obj = Window::instance().get_mesh_obj(mesh_id);
        if (obj == nullptr)
            return;

        m_highlight_shader->bind();

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        glm::mat4 positionOffset = glm::translate(-obj->get_data().offset);
        glm::mat4 transform = data.camera.world * obj->get_data().get_transform() * positionOffset;

//        auto highlights = obj->get_highlights();
//
//        for (auto highlight: highlights) {
//            auto entry = highlight.second;
//
//            OpenVolumeMesh::VertexHandle v_h = entry.v_h;
//            float red = entry.color.r;
//            float green = entry.color.g;
//            float blue = entry.color.b;
//            float alpha = entry.color.a;
//
//            auto vertex = Window::instance().get_mesh_obj(0)->m_mesh->vertex(v_h);
//
//            // set highlight properties
//            glm::vec4 vertex_pos = glm::vec4(vertex[0], vertex[1], vertex[2], 1.0f);
//            glm::vec4 highlight_color = glm::vec4(red, green, blue, alpha);
//            float highlight_scale = 0.03f;
//
//            m_highlight_shader->set_uniform_float("u_scale", highlight_scale);
//            m_highlight_shader->set_uniform_vec4f("u_position", vertex_pos);
//            m_highlight_shader->set_uniform_mat4f("u_transform", transform);
//            m_highlight_shader->set_uniform_mat4f("u_projection", data.camera.projection);
//            m_highlight_shader->set_uniform_mat4f("u_view", data.camera.view);
//            m_highlight_shader->set_uniform_vec4f("u_highlight_color", highlight_color);
//
//            m_vao->draw();
//
//        }

        glDisable(GL_BLEND);

        m_highlight_shader->unbind();
    }
}