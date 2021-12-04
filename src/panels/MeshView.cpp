
#include "glad/glad.h"

#include "MeshView.h"
#include "../input/Input.h"
#include "LogWindow.h"
#include "../Window.h"

#include <algorithm>
#include <cmath>

#include "imgui.h"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/vec_swizzle.hpp"

#include "../mesh/MeshObject.h"
#include "../rendering/shapes/Box.h"

namespace vOS
{
    MeshView::MeshView(int width, int height) :
            m_viewportPanelWidth(width),
            m_viewportPanelHeight(height),
            m_lastDown(false),
            m_lastX(0.0),
            m_lastY(0.0),
            m_arcBallOn(false)
    {
        m_meshFrameBuffer = new FrameBufferObject(width, height);
        m_selectionFrameBuffer = new FrameBufferObject(width, height);

        m_render_data.camera.position = glm::vec3{0.0f, 0.0f, 10.0f};
        m_render_data.light.color = glm::vec3{1.0f, 1.0f, 1.0f};
        m_render_data.mesh.color = glm::vec3{1.0f, 1.0f, 1.0f};

        // set up the initial camera position, direction and orientation of the mesh
        glm::mat4 position = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 scale = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
        glm::mat4 rotation = glm::mat4(1.0f);
        m_render_data.mesh.transform = position * rotation * scale;
        m_render_data.camera.world = glm::mat4(1.0f);
        m_render_data.camera.projection = glm::perspective(
                glm::radians(50.0f),
                (float) m_viewportPanelWidth / (float) m_viewportPanelHeight,
                0.001f,
                100000.0f
        );

        m_render_data.camera.view = glm::lookAt(
                m_render_data.camera.position,
                glm::vec3{0.0f, 0.0f, 0.0f},
                glm::vec3{0.0f, 1.0f, 0.0f}
        );

        glm::mat4 inverse = glm::inverse(m_render_data.camera.view);
        glm::vec3 view_dir = {inverse[2][0], inverse[2][1], inverse[2][2]};
        m_render_data.light.position = m_render_data.camera.position + glm::normalize(view_dir) * 10.0f;
    }

    MeshView::~MeshView()
    {
        delete m_meshFrameBuffer;
        delete m_selectionFrameBuffer;
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
            m_selectionFrameBuffer->resize(m_viewportPanelWidth, m_viewportPanelHeight);
            m_render_data.camera.projection = glm::perspective(
                    glm::radians(50.0f),
                    (float) m_viewportPanelWidth / (float) m_viewportPanelHeight,
                    0.001f,
                    100000.0f
            );
        }
    }

    glm::vec3 MeshView::get_arc_ball_vector(float x, float y) const
    {
        glm::vec3 res = glm::vec3(
                x / (float) m_viewportPanelWidth * 2.0f - 1.0f,
                y / (float) m_viewportPanelHeight * 2.0f - 1.0f,
                0.0f
        );
        res.y = -res.y;
        float squared = res.x * res.x + res.y * res.y;
        if (squared <= 1.0f)
        {
            res.z = (float) sqrt(1.0 - squared);
        } else
        {
            res = glm::normalize(res);
        }
        return res;
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

        if (!ImGui::IsWindowHovered() && !m_arcBallOn)
        {
            return;
        }

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
            glm::mat4 transform = glm::scale(
                    m_render_data.mesh.transform,
                    glm::vec3(1.0f + (float) Input::getScrollOffsetY() * scaleSpeed)
            );
            m_render_data.mesh.transform = transform;
        }
        m_lastDown = isDown;

        if (m_arcBallOn)
        {
            float speed = 0.05;

            double dx = mousePos.x - m_lastX;
            double dy = mousePos.y - m_lastY;

            if (std::abs(dx) > 0.0 || std::abs(dy) > 0.0)
            {
                glm::vec3 a = get_arc_ball_vector((float) m_lastX, (float) m_lastY);
                glm::vec3 b = get_arc_ball_vector(mousePos.x, mousePos.y);
                float angle = (float) std::acos(std::min(1.0f, glm::dot(a, b)));
                glm::vec3 axis_camera = glm::cross(a, b);
                glm::mat3 camera_to_object = glm::inverse(
                        glm::mat3(m_render_data.camera.view) * glm::mat3(m_render_data.camera.world));
                glm::vec3 axis_object = camera_to_object * axis_camera;
                m_render_data.camera.world = glm::rotate(m_render_data.camera.world, glm::degrees(angle) * speed,
                                                         axis_object);
            }
        }
        m_lastX = mousePos.x;
        m_lastY = mousePos.y;
    }

    void MeshView::renderMesh()
    {


        if (ImGui::IsKeyPressed(GLFW_KEY_W))
        {
            m_mesh_pass.set_wireframe_mode(!m_mesh_pass.get_wireframe_mode());
        }

        // now render our mesh scene to the framebuffer texture
        m_meshFrameBuffer->bind();

        // we need to clear our framebuffer as well
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_background_pass.render(*Window::instance().get_mesh_obj()->get_vao(), m_render_data);


        if (ImGui::IsKeyPressed(GLFW_KEY_S))
        {
            Window::instance().set_mesh_active(0);
        }
        if (ImGui::IsKeyPressed(GLFW_KEY_F))
        {
            Window::instance().set_mesh_active(1);
        }

        m_render_data.mesh.offset = Window::instance().get_mesh_obj()->get_mesh_offset();

        for (const std::pair<int, MeshObject *> m: Window::instance().get_mesh_list())
        {
            auto mesh = m.second;

            mesh->update_vertex_buffer();

            // render all passes
            if (mesh->get_vao() != nullptr)
            {
                m_mesh_pass.render(*mesh->get_vao(), m_render_data);
                m_shape_pass.render(*mesh->get_vao(), m_render_data);
                m_highlight_pass.render(*mesh->get_vao(), m_render_data);
            }
        }

        m_meshFrameBuffer->unbind();
    }

    void MeshView::renderSelection()
    {
        // now render our mesh scene to the framebuffer texture
        m_selectionFrameBuffer->bind();

        // we need to clear our framebuffer as well
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // render selection_pass for each mesh
        for (const std::pair<int, MeshObject *> m: Window::instance().get_mesh_list())
        {
            auto mesh = m.second;

            if (mesh->get_vao() != nullptr)
            {
                m_render_data.mesh.selection_offset = std::get<0>(mesh->selection_offset());
                m_selection_pass.render(*mesh->get_vao(), m_render_data);
            }
        }


        // check for actual picking
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            glFlush();
            glFinish();

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            // viewport (0,0) starts top left, but framebuffer (0,0) starts bottom left
            // viewport[3] equals viewport height
            GLint viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);

            // read Pixel data/color from framebuffer
            ImVec2 screen_pos = ImGui::GetCursorScreenPos();
            unsigned char data[4];
            glReadPixels(m_lastX - screen_pos.x, viewport[3] - (m_lastY - screen_pos.y), 1, 1, GL_RGBA,
                         GL_UNSIGNED_BYTE, data);

            // evaluate ID out of color
            int pickedID = data[0] + data[1] * 256 + data[2] * 256 * 256; //+ data[3] *256*256*256;

            // evaluate which in which mesh the color was selected
            auto mesh = Window::instance().get_mesh_obj();
            int from = 0;
            int to = 0;
            for (const std::pair<int, MeshObject *> m: Window::instance().get_mesh_list())
            {
                mesh = m.second;
                from = std::get<0>(mesh->selection_offset());
                to = std::get<1>(mesh->selection_offset());

                if (pickedID >= from && pickedID <= to)
                {
                    std::cout << "Mesh " << m.first << " was picked" << std::endl;

                    // because of unsigned int as return value mesh.to_faceID(pickedID) returns the id + 1 and 0 means
                    // there is no valid ID (e.g when clicking background)
                    unsigned int faceID = mesh->to_faceID(pickedID - from) - 1;

                    OpenVolumeMesh::FaceHandle face(faceID);
                    if (face.is_valid())
                    {

                        auto pick_pos = mesh->m_mesh->barycenter(face);
                        Box *shape = new Box(0.1f, 0.1f, 0.1f);
                        shape->set_position(pick_pos[0], pick_pos[1], pick_pos[2]);
                        shape->set_base_color(1.0f, 0.0f, 0.0f);
                        ShapePass::add_shape(shape);
                        std::cout << "pos x: " << pick_pos[0] << ", y: " << pick_pos[1] << ", z: " << pick_pos[2]
                                  << std::endl;
                        std::cout << "x: " << m_lastX - screen_pos.x << ", y: " << m_lastY - screen_pos.y << std::endl;
                    }

                    continue;
                }
            }
        }

        m_selectionFrameBuffer->unbind();
    }

    void MeshView::show()
    {
        auto padding = ImGui::GetStyle().WindowPadding;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});
        ImGui::Begin("Mesh");

        // handle the things related to our mesh rendering canvas
        handleResize();
        handleMouseControl();
        renderMesh();
        renderSelection();

        // store the current top left position, so we can draw text here later on top of our canvas
        auto topLeft = ImGui::GetCursorPos();
        topLeft.x += padding.x;
        topLeft.y += padding.y;

        // finally, add the framebuffer texture as an image to the imgui window
        ImGui::GetWindowDrawList()->AddImage(
                reinterpret_cast<ImTextureID>(m_meshFrameBuffer->get_texture_id()),
                ImGui::GetCursorScreenPos(),
                {ImGui::GetCursorScreenPos().x + (float) m_viewportPanelWidth,
                 ImGui::GetCursorScreenPos().y + (float) m_viewportPanelHeight},
                {0.0f, 1.0f},
                {1.0f, 0.0f}
        );

        // show frame time and fps
        ImGui::SetCursorPos(topLeft);
        ImGui::Text("%.3f ms", 1000.0f / ImGui::GetIO().Framerate);
        ImGui::SetCursorPos({ImGui::GetCursorPos().x + padding.x, ImGui::GetCursorPos().y});
        ImGui::Text("%.1f fps", ImGui::GetIO().Framerate);

        if (Window::instance().get_mesh_obj()->m_mesh != nullptr)
        {
            ImGui::SetCursorPos({ImGui::GetCursorPos().x + padding.x, ImGui::GetCursorPos().y});
            ImGui::Text("vertices: %zu", Window::instance().get_mesh_obj()->m_mesh->n_vertices());
            ImGui::SetCursorPos({ImGui::GetCursorPos().x + padding.x, ImGui::GetCursorPos().y});
            ImGui::Text("edges: %zu", Window::instance().get_mesh_obj()->m_mesh->n_edges());
            ImGui::SetCursorPos({ImGui::GetCursorPos().x + padding.x, ImGui::GetCursorPos().y});
            ImGui::Text("faces: %zu", Window::instance().get_mesh_obj()->m_mesh->n_faces());
        }

        ImGui::End();
        ImGui::PopStyleVar();
    }
}
