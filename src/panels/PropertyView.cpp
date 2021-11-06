
#include "glad/glad.h"

#include "PropertyView.h"
#include "../algorithms/VosWindow.h"

#include "glm/gtx/transform.hpp"
#include <iostream>

namespace vOS
{

    vOS::PropertyView::PropertyView(const MeshView& mesh_view): m_mesh_view(mesh_view)
    {
        std::vector<float> quad_vertices = {
                -0.5, 0.5, 0.0,
                -0.5, -0.5, 0.0,
                0.5, -0.5, 0.0,
                0.5, 0.5, 0.0
        };
        std::vector<unsigned int> quad_indices = {
                0, 1, 2,
                2, 3, 0
        };
        std::vector<float> quad_texture_coordinates = {
                0.0, 0.0,
                0.0, 1.0,
                1.0, 1.0,
                1.0, 0.0
        };
        m_vao = new VertexArrayObject(quad_vertices, quad_indices, quad_texture_coordinates, "texture");
        std::filesystem::path shaderPath = "shaders";
        m_shader = new Shader(shaderPath / "property.vert", shaderPath / "property.frag");
    }

    PropertyView::~PropertyView()
    {
        delete m_vao;
        delete m_shader;
    }

    void vOS::PropertyView::show()
    {
        m_mesh_view.m_meshFrameBuffer->bind();
        m_shader->bind();

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        // TODO: Get highlight vertices here

        auto highlights = VosWindow::instance().get_mesh_obj().get_highlights();
        for(int i = 0; i < highlights.size(); i++)
        {
            auto entry = highlights[i];

            OpenVolumeMesh::VertexHandle v_h = std::get<0>(entry);
            float red = std::get<1>(entry);
            float green = std::get<2>(entry);
            float blue = std::get<3>(entry);
            float alpha = std::get<4>(entry);

            auto vertex = VosWindow::instance().get_mesh_obj().m_mesh->vertex(v_h);

            // set highlight properties
            glm::vec4 vertex_pos = glm::vec4(vertex[0], vertex[1], vertex[2], 1.0f);
            glm::vec4 highlight_color = glm::vec4(red, green, blue, alpha);
            float highlight_scale = 0.03f;

            // calculate vertex transform
            glm::mat4 positionOffset = glm::translate(-VosWindow::instance().get_mesh_obj().get_mesh_offset());
            glm::mat4 transform = m_mesh_view.m_meshWorld * m_mesh_view.m_meshTransform * positionOffset;

            m_shader->setUniform1f("u_scale", highlight_scale);
            m_shader->setUniform4f("u_position", vertex_pos);
            m_shader->setUniformMat4f("u_transform", transform);
            m_shader->setUniformMat4f("u_projection", m_mesh_view.m_meshProjection);
            m_shader->setUniformMat4f("u_view", m_mesh_view.m_meshView);
            m_shader->setUniform4f("u_highlight_color", highlight_color);

            m_vao->draw();

        }
        /*
        auto vertices = VosWindow::instance().get_mesh_obj().vertices();
        for (int i = 0; i < vertices.size(); i += 3)
        {
            // set highlight properties
            glm::vec4 vertex_pos = glm::vec4(vertices[i], vertices[i + 1], vertices[i + 2], 1.0f);
            glm::vec4 highlight_color = glm::vec4(0.2, 1.0, 0.2, 1.0);
            float highlight_scale = 0.03f;

            // calculate vertex transform
            glm::mat4 positionOffset = glm::translate(-VosWindow::instance().get_mesh_obj().get_mesh_offset());
            glm::mat4 transform = m_mesh_view.m_meshWorld * m_mesh_view.m_meshTransform * positionOffset;

            m_shader->setUniform1f("u_scale", highlight_scale);
            m_shader->setUniform4f("u_position", vertex_pos);
            m_shader->setUniformMat4f("u_transform", transform);
            m_shader->setUniformMat4f("u_projection", m_mesh_view.m_meshProjection);
            m_shader->setUniformMat4f("u_view", m_mesh_view.m_meshView);
            m_shader->setUniform4f("u_highlight_color", highlight_color);

            m_vao->draw();

        }
        */
        glDisable(GL_BLEND);

        m_shader->unbind();
        m_mesh_view.m_meshFrameBuffer->unbind();
    }
}