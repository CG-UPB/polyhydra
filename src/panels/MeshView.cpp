
#include "glad/glad.h"

#include "MeshView.h"
#include "../input/Input.h"
#include "LogWindow.h"
#include "../algorithms/VosWindow.h"

#include <algorithm>

#include "imgui.h"
#include "glm/gtx/transform.hpp"

#include "../mesh/MeshObject.h"

namespace vOS
{
    MeshView::MeshView(int width, int height, FrameBufferObject& fbo):
            m_viewportPanelWidth(width),
            m_viewportPanelHeight(height),
            m_lastDown(false),
            m_lastX(0.0),
            m_lastY(0.0),
            m_arcBallOn(false),
            m_meshFrameBuffer(&fbo)
    {
        std::filesystem::path shaderPath = "shaders";
        m_meshShader = new Shader(shaderPath / "mesh.vert", shaderPath / "mesh.frag");

        // set up the initial camera position, direction and orientation of the mesh
        glm::mat4 position = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 scale = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
        glm::mat4 rotation = glm::mat4(1.0f);
        m_meshTransform = position * rotation * scale;
        m_meshWorld = glm::mat4(1.0f);
        m_meshProjection = glm::perspective(
                glm::radians(50.0f),
                (float) m_viewportPanelWidth / (float) m_viewportPanelHeight,
                0.001f,
                100000.0f
        );

        m_meshView = glm::lookAt(
                glm::vec3{0.0f, 0.0f, 10.0f},
                glm::vec3{0.0f, 0.0f, 0.0f},
                glm::vec3{0.0f, 1.0f, 0.0f}
        );
    }

    MeshView::~MeshView()
    {
        delete m_meshFrameBuffer;
        delete m_meshShader;
    }

    void MeshView::handleResize()
    {
        // if our window panel size changes, we need to adjust the framebuffer size and projection
        auto viewPortPanelSize = ImGui::GetContentRegionAvail();
        float width = std::max(viewPortPanelSize.x, 100.0f);
        float height = std::max(viewPortPanelSize.y, 100.0f);
        if (width != (float) m_viewportPanelWidth || height != (float) m_viewportPanelHeight)
        {
            m_viewportPanelWidth = (int) width;
            m_viewportPanelHeight = (int) height;
            m_meshFrameBuffer->resize(m_viewportPanelWidth, m_viewportPanelHeight);
            m_meshProjection = glm::perspective(
                    glm::radians(50.0f),
                    (float) m_viewportPanelWidth / (float) m_viewportPanelHeight,
                    0.001f,
                    100000.0f
            );
        }
    }

    void MeshView::handleMouseControl()
    {
        // check where the imgui window is inside the main window, and how big it is
        ImVec2 vMin = ImGui::GetWindowContentRegionMin();
        ImVec2 vMax = ImGui::GetWindowContentRegionMax();
        vMin.x += ImGui::GetWindowPos().x;
        vMin.y += ImGui::GetWindowPos().y;
        vMax.x += ImGui::GetWindowPos().x;
        vMax.y += ImGui::GetWindowPos().y;
        glm::vec2 mousePos = {Input::getMouseX(), Input::getMouseY()};

        bool isDown = Input::isKeyDown(GLFW_MOUSE_BUTTON_LEFT);

        // the cursor is inside the mesh viewport, so now we can manipulate the mesh view
        if (mousePos.x > vMin.x && mousePos.x < vMax.x && mousePos.y > vMin.y && mousePos.y < vMax.y)
        {
            // arc ball behavior
            if (isDown && !m_lastDown)
            {
                m_arcBallOn = true;
                m_lastX = mousePos.x;
                m_lastY = mousePos.y;
            }

            if (!isDown)
            {
                m_arcBallOn = false;
            }

            // scroll scaling of the mesh
            float scaleSpeed = 0.1f;
            m_meshTransform = glm::scale(
                    m_meshTransform,
                    glm::vec3(1.0f + (float) Input::getScrollOffsetY() * scaleSpeed)
            );
        }
        m_lastDown = isDown;

        // arc ball rotation based on this article: https://nerdhut.de/2019/12/04/arcball-camera-opengl/
        if (m_arcBallOn)
        {
            double dx = mousePos.x - m_lastX;
            double dy = mousePos.y - m_lastY;

            float scaleX = (float) std::abs(dx) / (float) m_viewportPanelWidth;
            float scaleY = (float) std::abs(dy) / (float) m_viewportPanelHeight;
            float rotSpeed = 350.0f;

            // when the camera is upside down, left and right dragging is swapped, so we need to check which direction
            // the camera is facing and negate the rotation direction if needed
            glm::mat4 inverseView = glm::inverse(m_meshView);
            glm::vec3 viewDir = {inverseView[2][0], inverseView[2][1], inverseView[2][2]};
            float rotX = rotSpeed * scaleX;
            if (viewDir.z < 0)
            {
                rotX *= -1.0f;
            }

            // rotate the world (all objects) around the y axis
            if (dx < 0)
            {
                m_meshWorld = glm::rotate(
                        m_meshWorld,
                        glm::radians(-rotX),
                        glm::vec3(0.0f, 1.0f, 0.0f)
                );
            }
            else if (dx > 0)
            {
                m_meshWorld = glm::rotate(
                        m_meshWorld,
                        glm::radians(rotX),
                        glm::vec3(0.0f, 1.0f, 0.0f)
                );
            }

            // rotate the camera around the x axis
            if (dy < 0)
            {
                m_meshView = glm::rotate(
                        m_meshView,
                        glm::radians(-rotSpeed * scaleY),
                        glm::vec3(1.0f, 0.0f, 0.0f)
                );
            }
            else if (dy > 0)
            {
                m_meshView = glm::rotate(
                        m_meshView,
                        glm::radians(rotSpeed * scaleY),
                        glm::vec3(1.0f, 0.0f, 0.0f)
                );
            }
            m_lastX = mousePos.x;
            m_lastY = mousePos.y;
            //LogWindow::getInstance()->addLog("Neue Position");
        }
    }

    void MeshView::renderMesh()
    {


        // render our mesh in polygon mode for debugging
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnable(GL_LINE_SMOOTH);
        glLineWidth(1);

        // now render our mesh scene to the framebuffer texture
        m_meshFrameBuffer->bind();

        // we need to clear our framebuffer as well
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        m_meshShader->bind();

        glm::mat4 positionOffset = glm::translate(- VosWindow::instance().get_mesh_obj().get_mesh_offset());
        glm::mat4 transform = m_meshWorld * m_meshTransform * positionOffset;

        // set all of our uniforms
        m_meshShader->setUniformMat4f("u_Transform", transform);
        m_meshShader->setUniformMat4f("u_Projection", m_meshProjection);
        m_meshShader->setUniformMat4f("u_View", m_meshView);

        // now draw to the actual texture of the framebuffer

        VosWindow::instance().get_mesh_obj().draw();

        // unbind our framebuffer and shader
        m_meshShader->unbind();
        m_meshFrameBuffer->unbind();

        // disable polygon mode again
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void MeshView::show()
    {
        ImGui::Begin("Mesh");

        // handle the things related to our mesh rendering canvas
        handleResize();
        handleMouseControl();
        renderMesh();

        // store the current top left position, so we can draw text here later on top of our canvas
        auto topLeft = ImGui::GetCursorPos();
        auto padding = ImGui::GetStyle().WindowPadding;
        topLeft.x += padding.x;
        topLeft.y += padding.y;

        // finally, add the framebuffer texture as an image to the imgui window
        ImGui::GetWindowDrawList()->AddImage(
                reinterpret_cast<ImTextureID>(m_meshFrameBuffer->getTextureID()),
                ImGui::GetCursorScreenPos(),
                {ImGui::GetCursorScreenPos().x + (float) m_viewportPanelWidth,
                 ImGui::GetCursorScreenPos().y + (float) m_viewportPanelHeight},
                {0.0f, 1.0f},
                {1.0f, 0.0f}
        );

        // show frame time and fps
        /*ImGui::SetCursorPos(topLeft);
        ImGui::Text("%.3f ms", 1000.0f / ImGui::GetIO().Framerate);
        ImGui::SetCursorPos({ImGui::GetCursorPos().x + padding.x, ImGui::GetCursorPos().y});
        ImGui::Text("%.1f fps", ImGui::GetIO().Framerate);
        ImGui::SetCursorPos({ImGui::GetCursorPos().x + padding.x, ImGui::GetCursorPos().y});
        ImGui::Text("vertices: %zu", VosWindow::instance().get_mesh_obj().m_mesh->n_vertices());
        ImGui::SetCursorPos({ImGui::GetCursorPos().x + padding.x, ImGui::GetCursorPos().y});
        ImGui::Text("edges: %zu", VosWindow::instance().get_mesh_obj().m_mesh->n_edges());
        ImGui::SetCursorPos({ImGui::GetCursorPos().x + padding.x, ImGui::GetCursorPos().y});
        ImGui::Text("faces: %zu", VosWindow::instance().get_mesh_obj().m_mesh->n_faces());
*/
        ImGui::End();
    }
}
