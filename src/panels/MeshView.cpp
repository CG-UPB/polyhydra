//
// Created by steffen on 11.10.21.
//

#include "glm/gtx/transform.hpp"

#include "iostream"

#include "MeshView.h"
#include "imgui.h"

namespace vOS
{
    void MeshView::show()
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // update and render mesh
        m_meshFrameBuffer->bind();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        m_meshShader->bind();

        glm::mat4 position = glm::translate(glm::vec3(m_meshPosition[0], m_meshPosition[1], m_meshPosition[2]));
        glm::mat4 scale = glm::scale(glm::vec3(m_meshScale[0], m_meshScale[1], m_meshScale[2]));
        glm::vec3 rotation = glm::normalize(glm::vec3(m_meshRotation[0], m_meshRotation[1], m_meshRotation[2]));
        glm::mat4 transform = glm::rotate(glm::mat4(1.0f), m_meshRotation[3], rotation) * position * scale;
        m_meshShader->setUniformMat4f("u_Transform", transform);

        m_vertexArrayObject->bind();
        m_vertexArrayObject->draw();
        m_vertexArrayObject->unbind();
        m_meshShader->unbind();
        m_meshFrameBuffer->unbind();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        ImGui::PushStyleColor(ImGuiCol_ChildBg, {0.0f, 0.0f, 0.0f, 1.0f});
        ImGui::Begin("Mesh");

        auto viewPortPanelSize = ImGui::GetContentRegionAvail();
        float width = std::max(viewPortPanelSize.x, 100.0f);
        float height = std::max(viewPortPanelSize.y, 100.0f);
        if (width != (float) m_viewportPanelWidth || height != (float) m_viewportPanelHeight)
        {
            m_viewportPanelWidth = (int) width;
            m_viewportPanelHeight = (int) height;
            m_meshFrameBuffer->resize(m_viewportPanelWidth, m_viewportPanelHeight);
        }
        ImGui::GetWindowDrawList()->AddImage(
                reinterpret_cast<ImTextureID>(m_meshFrameBuffer->getTextureID()),
                ImGui::GetCursorScreenPos(),
                {ImGui::GetCursorScreenPos().x + (float) m_viewportPanelWidth,
                 ImGui::GetCursorScreenPos().y + (float) m_viewportPanelHeight},
                {0.0f, 1.0f},
                {1.0f, 0.0f}
        );

        if (ImGui::DragFloat3("Position", m_meshPosition, 0.05f)) {}
        if (ImGui::DragFloat3("Scale", m_meshScale, 0.05f)) {}
        if (ImGui::DragFloat4("Rotation", m_meshRotation, 0.05f)) {}

        ImGui::End();
        ImGui::PopStyleColor();
    }

    MeshView::MeshView(int width, int height) :
            m_viewportPanelWidth(width),
            m_viewportPanelHeight(height)
    {
        std::vector<float> vertices = {
                // front
                -1.0, -1.0, 1.0,
                1.0, -1.0, 1.0,
                1.0, 1.0, 1.0,
                -1.0, 1.0, 1.0,
                // back
                -1.0, -1.0, -1.0,
                1.0, -1.0, -1.0,
                1.0, 1.0, -1.0,
                -1.0, 1.0, -1.0
        };
        std::vector<unsigned int> indices = {
                // front
                0, 1, 2,
                2, 3, 0,
                // right
                1, 5, 6,
                6, 2, 1,
                // back
                7, 6, 5,
                5, 4, 7,
                // left
                4, 0, 3,
                3, 7, 4,
                // bottom
                4, 5, 1,
                1, 0, 4,
                // top
                3, 2, 6,
                6, 7, 3
        };
        m_vertexArrayObject = new VertexArrayObject(vertices, indices);
        m_meshFrameBuffer = new FrameBufferObject(width, height);
        m_meshShader = new Shader("shaders/mesh.vert", "shaders/mesh.frag");
        m_meshPosition[0] = 0.0f;
        m_meshPosition[1] = 0.0f;
        m_meshPosition[2] = 0.0f;
        m_meshScale[0] = 0.5f;
        m_meshScale[1] = 0.5f;
        m_meshScale[2] = 0.5f;
        m_meshRotation[0] = 1.0f;
        m_meshRotation[1] = 0.0f;
        m_meshRotation[2] = 0.0f;
        m_meshRotation[3] = 0.0f;
    }

    MeshView::~MeshView()
    {
        delete m_vertexArrayObject;
        delete m_meshFrameBuffer;
        delete m_meshShader;
    }
}
