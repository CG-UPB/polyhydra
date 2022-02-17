
#include "glad/glad.h"

#include "../util/StringUtil.h"
#include "MeshView.h"
#include "../input/Input.h"
#include "LogWindow.h"

#include <cmath>
#include <fstream>

#include "imgui.h"
#include "glm/gtx/transform.hpp"

#include "../mesh/MeshObject.h"

#include "../rendering/meshes/CommonMeshes.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


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


        m_meshFrameBuffer = new FrameBufferObject(width, height, false);
        m_selectionFrameBuffer = new FrameBufferObject(width / 2, height / 2);
        m_pixel_buffer = new PixelBufferObject(2, width / 2, height / 2);
        m_screen_quad_frameBuffer = new FrameBufferObject(width, height);
        m_pre_pass_framebuffer = new PrePassFrameBufferObject(width, height);

        m_mesh_pass = new MeshPass(this);
        m_transparency_pass_wb = new TransparencyPass_WB(this, width, height);
        m_transparency_pass_dp = new TransparencyPass_DP(this, width, height);

        m_render_data.camera.position = glm::vec3{0.0f, 0.0f, 10.0f};
        m_render_data.light.color = glm::vec3{1.0f, 1.0f, 1.0f};

        // set up the initial camera position, direction and orientation of the mesh
        m_render_data.camera.world = glm::mat4(1.0f);
        m_render_data.camera.projection = glm::perspective(
                glm::radians(60.0f),
                (float) m_viewportPanelWidth / (float) m_viewportPanelHeight,
                0.1f,
                500.0f
        );

        m_render_data.camera.view = glm::lookAt(
                m_render_data.camera.position,
                glm::vec3{0.0f, 0.0f, 0.0f},
                glm::vec3{0.0f, 1.0f, 0.0f}
        );

        glm::mat4 inverse = glm::inverse(m_render_data.camera.view);
        glm::vec3 view_dir = {inverse[2][0], inverse[2][1], inverse[2][2]};
        m_render_data.light.position = m_render_data.camera.position + glm::normalize(view_dir) * 20.0f;

        m_zoom = false;
        m_zoom_point = glm::vec3(0, 0, 0);
    }

    MeshView::~MeshView()
    {
        delete m_meshFrameBuffer;
        delete m_selectionFrameBuffer;
        delete m_pixel_buffer;
        delete m_mesh_pass;
        delete m_transparency_pass_wb;
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
            m_screen_quad_frameBuffer->resize(m_viewportPanelWidth, m_viewportPanelHeight);
            m_pre_pass_framebuffer->resize(m_viewportPanelWidth, m_viewportPanelHeight);
            m_transparency_pass_wb->resize_buffers(m_viewportPanelWidth, m_viewportPanelHeight);
            m_transparency_pass_dp->resize_buffers(m_viewportPanelWidth, m_viewportPanelHeight);
            m_selectionFrameBuffer->resize(m_viewportPanelWidth / 2, m_viewportPanelHeight / 2);
            delete m_pixel_buffer;
            m_pixel_buffer = new PixelBufferObject(2, m_viewportPanelWidth / 2, m_viewportPanelHeight / 2);
            m_render_data.camera.projection = glm::perspective(
                    glm::radians(50.0f),
                    (float) m_viewportPanelWidth / (float) m_viewportPanelHeight,
                    0.1f,
                    500.0f
            );
        }
    }

    glm::vec3 MeshView::get_arc_ball_vector(float x, float y) const
    {
        auto viewport_start = ImGui::GetCursorScreenPos();
        glm::vec3 res = glm::vec3(
                (x - viewport_start.x) / (float) m_viewportPanelWidth * 1.5f - 0.75f,
                (y - viewport_start.y) / (float) m_viewportPanelHeight * 1.5f - 0.75f,
                0.0f
        );
        res.y = -res.y;
        float squared = res.x * res.x + res.y * res.y;
        if (squared <= 1.0f)
        {
            res.z = (float) sqrt(1.0f - squared);
        }
        else
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
        glm::vec2 mousePos = {Input::get_mouse_X(), Input::get_mouse_Y()};

        if (!ImGui::IsWindowHovered() && !m_arcBallOn)
        {
            return;
        }

        bool isDown = Input::mouse_pressed();

        // Move camera in direction of Movement Vector (WASD movement)

        auto movement_vector = glm::vec3 (Input::get_wasd_movement_vector_X(),Input::get_wasd_movement_vector_Y(),Input::get_wasd_movement_vector_Z());

        // Reset Movement speed multiplier whenever we stop moving or when we start moving
        if((movement_vector[0] == 0 && movement_vector[1] == 0 && movement_vector[2] == 0) || (m_previous_movement_vector[0] == 0 && m_previous_movement_vector[1] == 0 && m_previous_movement_vector[2] == 0))
            m_movement_speed_multiplier = 1;

        m_previous_movement_vector[0] = movement_vector[0];
        m_previous_movement_vector[1] = movement_vector[1];
        m_previous_movement_vector[2] = movement_vector[2];

        float movement_speed = m_movement_speed_multiplier;
        m_movement_speed_multiplier *= 1.1f; // Gradually speed up movement
        m_render_data.camera.position += movement_vector * movement_speed;

        //std::cout << m_render_data.camera.position[0] << " "  << m_render_data.camera.position[1] << " " << m_render_data.camera.position[2] << " " << std::endl;

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
            m_render_data.camera.world = glm::scale(
                    m_render_data.camera.world,
                    glm::vec3(1.0f + (float) Input::get_scroll_offset_Y() * scaleSpeed)
            );
        }
        m_lastDown = isDown;

        if (m_arcBallOn)
        {
            float speed = 0.04;

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

    void MeshView::renderMesh(int mesh_id)
    {

        // Get Mesh
        MeshObject *obj = Window::instance().get_mesh_obj(mesh_id);
        if (obj == nullptr)
            return;

        MeshData& mesh_data = obj->get_data();

        if(!mesh_data.m_visible)
        {
            return;
        }

        if(!m_zoom)
        {
            m_zoom_point = obj->get_mesh_offset();
        }
        mesh_data.offset = m_zoom_point;


        obj->update_vertex_buffer();

        // render all passes
        if (obj->get_vao() != nullptr) {
            m_mesh_pass->render(obj->get_vao(), m_render_data, mesh_id);
            m_highlight_pass.render(nullptr, m_render_data, mesh_id);
            m_shape_pass.render(nullptr, m_render_data, mesh_id);
        }
    }

    void MeshView::m_take_screenshot(const std::string& filename)
    {
        // export x times the original resolution -> we should make this configurable when taking a screenshot
        float resolution_upscale = 2.0f;

        int export_width = (int) ((float) m_viewportPanelWidth * resolution_upscale);
        int export_height = (int) ((float) m_viewportPanelHeight * resolution_upscale);

        auto export_framebuffer_ms = new FrameBufferObject(export_width, export_height, false);
        auto export_framebuffer = new FrameBufferObject(export_width, export_height);

        //render_pre_pass();

        export_framebuffer_ms->bind();

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto active_mesh = Window::instance().get_active_mesh_obj();
        if (active_mesh != nullptr)
        {
            if(!m_zoom)
            {
                m_zoom_point = Window::instance().get_active_mesh_obj()->get_mesh_offset();
            }
            Window::instance().get_active_mesh_obj()->get_data().offset = m_zoom_point;

            for(const std::pair<int, MeshObject*> m : Window::instance().get_mesh_list())
            {
                auto mesh = m.second;
                if(!mesh->get_data().m_visible)
                {
                    continue;
                }

                mesh->update_vertex_buffer();

                // render all passes
                if (mesh->get_vao() != nullptr) {
                    m_mesh_pass->render(mesh->get_vao(), m_render_data, m.first);
                    m_highlight_pass.render(nullptr, m_render_data, m.first);
                    m_shape_pass.render(nullptr, m_render_data, m.first);
                }
            }
        }

        glFlush();
        glFinish();

        export_framebuffer_ms->unbind();

        // copy our multisampled framebuffer to the output framebuffer
        FrameBufferObject::copy(export_framebuffer_ms, export_framebuffer);

        export_framebuffer->bind();

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        int sWidth = export_framebuffer->get_width();
        int sHeight = export_framebuffer->get_height();
        std::vector<char> buffer(4 * sWidth * sHeight);

        glReadPixels(0, 0, sWidth, sHeight, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());

        stbi_flip_vertically_on_write(true);

        auto split = StringUtil::split_str(filename, ".");
        std::string extension = split[split.size() - 1];

        if (extension == "bmp")
        {
            stbi_write_bmp(filename.c_str(), sWidth, sHeight, 4, buffer.data());
        }
        else if (extension == "png")
        {
            stbi_write_png(filename.c_str(), sWidth, sHeight, 4, buffer.data(), 4 * sWidth);
        }

        export_framebuffer->unbind();

        delete export_framebuffer_ms;
        delete export_framebuffer;
    }


    void MeshView::renderSelection()
    {
        // now render our mesh scene to the framebuffer texture
        m_selectionFrameBuffer->bind();

        // viewport (0,0) starts top left, but framebuffer (0,0) starts bottom left
        // viewport[3] equals viewport height
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        // read Pixel data/color from framebuffer
        ImVec2 screen_pos = ImGui::GetCursorScreenPos();

        GLubyte* data = m_pixel_buffer->start_read(
                (int) (m_lastX - screen_pos.x) / 2,
                (int) (viewport[3] * 2 - (m_lastY - screen_pos.y)) / 2,
                1,
                1
        );

        if (data != nullptr)
        {
            // evaluate ID out of color
            int type = data[0] & 3;
            int id;
            if (SelectionPass::DEBUG_MODE)
            {
                id = (data[0] + data[1] * 256 + data[2] * 256 * 256) >> 2;
            }
            else
            {
                id = (data[0] + data[1] * 256 + data[2] * 256 * 256 + data[3] * 256 * 256 * 256) >> 2;
            }
            querySelection(type,id);
        }

        m_pixel_buffer->finish_read();

        m_current_frame = (m_current_frame + 1) % m_frame_limit;
        if (m_current_frame == 0) {
            // we need to clear our framebuffer as well
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            for (const std::pair<int, MeshObject *> m: Window::instance().get_mesh_list()) {
                auto mesh = m.second;
                m_selection_pass.render_mesh(mesh, m_render_data, m.first);
            }
        }
        m_selectionFrameBuffer->unbind();

        m_meshFrameBuffer->bind();
        for (const std::pair<int, MeshObject *> m: Window::instance().get_mesh_list()) {
            m_selection_hover_pass.render(nullptr, m_render_data, m.first);
        }
        m_meshFrameBuffer->unbind();
    }

    void MeshView::render_pre_pass()
    {
        m_pre_pass_framebuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for(const std::pair<int, MeshObject*> m : Window::instance().get_mesh_list())
        {
            auto mesh = m.second;
            if(!mesh->get_data().m_visible)
            {
                continue;
            }
            mesh->update_vertex_buffer();
            if (mesh->get_vao() != nullptr) {
                m_pre_pass.render(mesh->get_vao(), m_render_data, m.first);
                continue;
            }
        }

        m_pre_pass_framebuffer->unbind();
    }

    void MeshView::render_transparency_wb()
    {

        glClear(GL_COLOR_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, m_transparency_pass_wb->m_transparent_framebuffer);
        m_transparency_pass_wb->clear_framebuffer();

        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunci(0, GL_ONE, GL_ONE);
        glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
        //glBlendFunci(2, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
        glBlendEquation(GL_FUNC_ADD);
        //glDisable(GL_CULL_FACE);

        for(const std::pair<int, MeshObject*> m : Window::instance().get_mesh_list())
        {
            auto mesh = m.second;

            MeshData& mesh_data = mesh->get_data();

            if(!mesh->get_data().m_visible)
            {
                continue;
            }

//            if(!m_zoom)
//            {
//                m_zoom_point = mesh->get_mesh_offset();
//            }
//            mesh_data.offset = m_zoom_point;

            mesh->update_vertex_buffer();

            if (mesh->get_vao() != nullptr) {
                m_transparency_pass_wb->render(mesh->get_vao(), m_render_data, m.first);
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        glDepthFunc(GL_ALWAYS);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        m_meshFrameBuffer->bind();
        m_transparency_pass_wb->render_composition();
        m_meshFrameBuffer->unbind();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void MeshView::render_transparency_dp()
    {
        int num_passes = 8;
        unsigned int framebuffer = m_transparency_pass_dp->m_transparent_framebuffer;
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        for( int i = 0; i < num_passes; i++)
        {

            // first render all meshes
            for(const std::pair<int, MeshObject*> m : Window::instance().get_mesh_list())
            {
                auto mesh = m.second;
                MeshData& mesh_data = mesh->get_data();
                if(!mesh->get_data().m_visible)
                {
                    continue;
                }
                if(!m_zoom)
                {
                    m_zoom_point = mesh->get_mesh_offset();
                }
                mesh_data.offset = m_zoom_point;

                mesh->update_vertex_buffer();

                if (mesh->get_vao() != nullptr) {
                    m_transparency_pass_dp->render(mesh->get_vao(), m_render_data, m.first);
                }
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // then composite the transparent and opaque result
        m_meshFrameBuffer->bind();
        m_transparency_pass_dp->render_composition();
        m_meshFrameBuffer->unbind();

    }

    void MeshView::querySelection(int type, int picked_id)
    {
        // evaluate which in which mesh the color was selected
        bool any_mesh_hovered = false;
        for (const auto& m: Window::instance().get_mesh_list())
        {
            auto mesh = m.second;
            int from = std::get<0>(mesh->selection_offset());
            int to = std::get<1>(mesh->selection_offset());

            //std::cout << "from: " << from << ", to: " << to << " picked_id: " << picked_id << std::endl;

            if (picked_id >= from && picked_id <= to)
            {
                m_hovered_element_id = picked_id;
                m_hovered_element_type = type;

                any_mesh_hovered = true;

                if (type == SELECTION_TYPE_FACE)
                {
                    // because of unsigned int as return value mesh.to_faceID(pickedID) returns the id + 1 and 0 means
                    // there is no valid ID (e.g when clicking background)
                    int face_id = mesh->to_faceID(picked_id - from) - 1;

                    //std::cout << "hovering face with id: " << face_id << std::endl;

                    m_selection_hover_pass.select(*mesh, m_render_data,m.first, type, face_id);

                    OpenVolumeMesh::FaceHandle face(face_id);
                    if (face.is_valid() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                    {
                        // Select element via Window class, to activate Callback function
                        // To avoid problems with the Callback functions, we unlock the mutex guard here and lock it again after the method is done
                        Window::instance().rendering_mutex.unlock();
                        Window::instance().select_element(m.first, face_id, type);
                        Window::instance().rendering_mutex.lock();
                    }

                }
                else if (type == SELECTION_TYPE_VERTEX)
                {
                    int vertex_id = mesh->to_vertexID(picked_id - from) - 1;

                    m_selection_hover_pass.select(*mesh, m_render_data,m.first, type, vertex_id);

                    OpenVolumeMesh::VertexHandle vertex(vertex_id);
                    if (vertex.is_valid() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                    {
                        // Select element via Window class, to activate Callback function
                        // To avoid problems with the Callback functions, we unlock the mutex guard here and lock it again after the method is done
                        Window::instance().rendering_mutex.unlock();
                        Window::instance().select_element(m.first, vertex_id, type);
                        Window::instance().rendering_mutex.lock();
                    }
                }
                else if (type == SELECTION_TYPE_EDGE)
                {
                    int edge_id = mesh->to_edgeID(picked_id - from) - 1;

                    m_selection_hover_pass.select(*mesh, m_render_data,m.first, type, edge_id);

                    OpenVolumeMesh::EdgeHandle edge(edge_id);
                    if (edge.is_valid() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                    {
                        // Select element via Window class, to activate Callback function
                        // To avoid problems with the Callback functions, we unlock the mutex guard here and lock it again after the method is done
                        Window::instance().rendering_mutex.unlock();
                        Window::instance().select_element(m.first, edge_id, type);
                        Window::instance().rendering_mutex.lock();
                    }
                }

                break;
            }

        }
        if(ImGui::IsWindowFocused() && ImGui::IsMouseDoubleClicked(0))
        {
            m_zoom_point = m_selection_hover_pass.m_zoom_point;
            m_zoom = true;
        }
        if(ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape)))
        {
            std::cout << "ESCAPE" << std::endl;
            m_zoom = false;
        }

        auto active_mesh = Window::instance().get_active_mesh_obj();
        if (!any_mesh_hovered && active_mesh != nullptr)
        {
            m_selection_hover_pass.select(*active_mesh, m_render_data, 0, 0, 0);
        }
    }

    void MeshView::set_zoom_point(glm::vec3 zoom_point)
    {
        m_zoom_point = zoom_point;
    }


    void MeshView::show()
    {
        if(GlobalViewerSettings::getInstance()->m_get_take_snapshot())
        {
            GlobalViewerSettings::getInstance()->m_set_take_snapshot(false);
            // Snapshot erstellen
            std::string filename = GlobalViewerSettings::getInstance()->m_get_actual_snapshot_filename();
            this->m_take_screenshot(filename);
        }

        auto padding = ImGui::GetStyle().WindowPadding;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});
        ImGui::Begin("Mesh");

        // handle the things related to our mesh rendering canvas
        handleResize();
        handleMouseControl();
        // Render Meshes
        render_pre_pass();

        // Now render our mesh scene to the framebuffer texture
        // Start with opaque objects

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        m_meshFrameBuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_background_pass.render(nullptr, m_render_data, 0);
        for (const auto& m: Window::instance().get_mesh_list())
        {
            renderMesh(m.first);
        }
        m_meshFrameBuffer->unbind();

        // Render transparent objects
        render_transparency_wb();
        //render_transparency_dp();


        if (GlobalViewerSettings::getInstance()->m_get_current_selection_activated()){
            renderSelection();
        }

        // set render states
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE); // enable depth writes so glClear won't ignore clearing the depth buffer
        glDisable(GL_BLEND);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // copy multisampled framebuffer that we rendered on to the imgui texture for display
        FrameBufferObject::copy(m_meshFrameBuffer, m_screen_quad_frameBuffer);

        // store the current top left position, so we can draw text here later on top of our canvas
        auto topLeft = ImGui::GetCursorPos();
        topLeft.x += padding.x;
        topLeft.y += padding.y;

        ImTextureID texture_id;
        if (SelectionPass::DEBUG_MODE)
        {
            texture_id = reinterpret_cast<ImTextureID>(m_selectionFrameBuffer->get_texture_id());
        }
        else
        {
            texture_id = reinterpret_cast<ImTextureID>(m_screen_quad_frameBuffer->get_texture_id());
        }

        //texture_id = reinterpret_cast<ImTextureID>(m_meshFrameBuffer->get_texture_id());
        //texture_id = reinterpret_cast<ImTextureID>(m_transparency_pass_wb->m_reveal_texture);
        //texture_id = reinterpret_cast<ImTextureID>(m_transparency_pass_dp->m_front_texture);


        // finally, add the framebuffer texture as an image to the imgui window
        ImGui::GetWindowDrawList()->AddImage(
                texture_id,
                ImGui::GetCursorScreenPos(),
                {ImGui::GetCursorScreenPos().x + (float) m_viewportPanelWidth,
                 ImGui::GetCursorScreenPos().y + (float) m_viewportPanelHeight},
                {0.0f, 1.0f},
                {1.0f, 0.0f}
        );

        // show frame time and fps
        ImGui::SetCursorPos(topLeft);
        ImGui::TextColored(ImVec4(0,0,0,1), "%.3f ms", 1000.0f / ImGui::GetIO().Framerate);
        ImGui::SetCursorPos({ImGui::GetCursorPos().x + padding.x, ImGui::GetCursorPos().y});
        ImGui::TextColored(ImVec4(0,0,0,1), "%.1f fps", ImGui::GetIO().Framerate);

        // Show hovered element type and id

        if (GlobalViewerSettings::getInstance()->m_get_current_selection_activated())
        {
            std::string hovered_element_name = m_hovered_element_type == 3 ? "Face" : (m_hovered_element_type == 1 ? "Vertex" :
                                                                                       (m_hovered_element_type == 2 ? "Edge" : "Cell"));
            hovered_element_name += " : ";
            hovered_element_name += std::to_string(m_hovered_element_id);

            ImGui::Text(hovered_element_name.c_str());
        }

        /*
        if (Window::instance().has_mesh() && Window::instance().get_active_mesh_obj() != nullptr &&  Window::instance().get_active_mesh_obj()->m_mesh != nullptr)
        {
            auto mesh = Window::instance().get_active_mesh_obj()->m_mesh;

            ImGui::SetCursorPos({ImGui::GetCursorPos().x + padding.x, ImGui::GetCursorPos().y});
            ImGui::Text("vertices: %zu", mesh->n_vertices());
            ImGui::SetCursorPos({ImGui::GetCursorPos().x + padding.x, ImGui::GetCursorPos().y});
            ImGui::Text("edges: %zu",mesh->n_edges());
            ImGui::SetCursorPos({ImGui::GetCursorPos().x + padding.x, ImGui::GetCursorPos().y});
            ImGui::Text("faces: %zu", mesh->n_faces());
        }*/

        ImGui::End();
        ImGui::PopStyleVar();
    }
}
