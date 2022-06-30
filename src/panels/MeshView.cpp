
#include "MeshView.h"
#include "../util/StringUtil.h"
#include "../input/Input.h"

#include <stb_image_write.h>

namespace volumeshOS::Internal
{
    MeshView::MeshView(int width, int height) :
            m_viewportPanelWidth(width),
            m_viewportPanelHeight(height),
            m_lastDown(false),
            m_lastX(0.0),
            m_lastY(0.0)
    {
        m_meshFrameBuffer = std::make_shared<FrameBufferObject>(width, height, FrameBufferObject::RGBA_AND_DEPTH_MULTISAMPLE);
        m_screen_quad_frameBuffer = std::make_shared<FrameBufferObject>(width, height, FrameBufferObject::RGBA_AND_DEPTH);
        renderer = std::make_shared<Renderer>(width, height, m_meshFrameBuffer, m_screen_quad_frameBuffer);
        renderer->set_selection_callback(std::bind(&MeshView::querySelection, this, std::placeholders::_1, std::placeholders::_2));

        // Set Camera Viewport Size
        m_render_data.camera.set_viewport_size(width, height);
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
            renderer->resize(m_viewportPanelWidth, m_viewportPanelHeight);
            m_render_data.camera.set_viewport_size(width, height);
        }
    }

    void MeshView::m_take_screenshot(const std::string& filename)
    {
        // export x times the original resolution -> we should make this configurable when taking a screenshot
        float resolution_upscale = 2.0f;

        int prev_width = m_viewportPanelWidth;
        int prev_height = m_viewportPanelHeight;

        int export_width = (int) ((float) m_viewportPanelWidth * resolution_upscale);
        int export_height = (int) ((float) m_viewportPanelHeight * resolution_upscale);

        // we need to do this since some passes need the current width and height for rendering
        m_viewportPanelWidth = export_width;
        m_viewportPanelHeight = export_height;

        auto export_framebuffer_ms = std::make_shared<FrameBufferObject>(export_width, export_height,FrameBufferObject::RGBA_AND_DEPTH_MULTISAMPLE);
        auto export_framebuffer = std::make_shared<FrameBufferObject>(export_width, export_height, FrameBufferObject::RGBA_AND_DEPTH);

        renderer->set_target_framebuffer(export_framebuffer_ms, export_framebuffer);
        renderer->resize(export_width, export_height);
        renderer->render(&m_render_data, false);

        glFlush();
        glFinish();

        export_framebuffer_ms->unbind();
        export_framebuffer->bind();

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        int sWidth = export_framebuffer->get_width();
        int sHeight = export_framebuffer->get_height();
        std::vector<unsigned char> buffer(4 * sWidth * sHeight);

        glReadPixels(0, 0, sWidth, sHeight, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());

        // since we are rendering on a transparent background, we need to undo the pre-multiplication
        for (size_t i = 0; i < buffer.size() / 4; i++)
        {
            float r = (float) ((int) buffer[i * 4]) / 255.0f;
            float g = (float) ((int) buffer[i * 4 + 1]) / 255.0f;
            float b = (float) ((int) buffer[i * 4 + 2]) / 255.0f;
            float a = (float) ((int) buffer[i * 4 + 3]) / 255.0f;
            float alpha = std::max(a, 0.00001f);
            buffer[i * 4] = (unsigned char) ((int) ((r / alpha) * 255.0f));
            buffer[i * 4 + 1] = (unsigned char) ((int) ((g / alpha) * 255.0f));
            buffer[i * 4 + 2] = (unsigned char) ((int) ((b / alpha) * 255.0f));
        }

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

        // restore the old width and height
        m_viewportPanelWidth = prev_width;
        m_viewportPanelHeight = prev_height;
        renderer->set_target_framebuffer(m_meshFrameBuffer, m_screen_quad_frameBuffer);
        renderer->resize(m_viewportPanelWidth, m_viewportPanelHeight);
    }

    void MeshView::querySelection(int type, int picked_id)
    {
        // evaluate which in which mesh the color was selected
        bool any_mesh_hovered = false;

        // Remember face id in case of double click
        int face_id = 0;
        int face_id_mesh = -1;
        int hovered_mesh_id = -1;

        for (const auto& [id, mesh] : Window::instance().get_mesh_list())
        {
            int from = std::get<0>(mesh->selection_offset());
            int to = std::get<1>(mesh->selection_offset());

            if (picked_id >= from && picked_id <= to)
            {
                m_hovered_element_id = picked_id;
                m_hovered_element_type = type;

                hovered_mesh_id = id;
                any_mesh_hovered = true;

                auto& settings = *GlobalViewerSettings::getInstance();

                if (type == SELECTION_TYPE_FACE)
                {
                    face_id_mesh = id;
                    int halfface_id = mesh->to_halfface_id(picked_id - from) - 1;
                    auto chf = OpenVolumeMesh::HalfFaceHandle{halfface_id};
                    auto ch = mesh->get_ovm()->incident_cell(chf);
                    mesh->get_mvb()->hover_halfface(halfface_id);
                    face_id = OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>::face_handle(chf).idx();

                    if (settings.get_isolation_state())
                    {
                        if (ch.is_valid() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                        {
                            mesh->get_mvb()->set_cell_isolated(ch.idx());
                        }
                    }

                    if (settings.get_selection_mode() == CELL || settings.get_digging_activated())
                    {
                        if (chf.is_valid())
                        {
                            auto cell = mesh->get_ovm()->incident_cell(chf);
                            mesh->get_mvb()->hover_cell(cell.idx());
                        }

                        if (settings.get_digging_activated() && !settings.get_isolation_state())
                        {
                            if (ch.is_valid() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                            {
                                mesh->get_mvb()->set_cell_digged(ch.idx(), true);
                            }
                        }
                        else
                        {
                            // cell_handle beinhaltet cell
                            if (ch.is_valid() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                            {
                                // Select element via Window class, to activate Callback function
                                // To avoid problems with the Callback functions, we unlock the mutex guard here and lock it again after the method is done
                                Window::instance().rendering_mutex.unlock();
                                Window::instance().select_element(id, ch.idx(), 6);
                                Window::instance().rendering_mutex.lock();
                            }
                        }
                    }
                    else
                    {
                        renderer->m_selection_hover_pass.hover(m_render_data, id, type, face_id);

                        OpenVolumeMesh::FaceHandle face(face_id);
                        if (face.is_valid() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                        {
                            // Select element via Window class, to activate Callback function
                            // To avoid problems with the Callback functions, we unlock the mutex guard here and lock it again after the method is done
                            Window::instance().rendering_mutex.unlock();
                            Window::instance().select_element(id, face_id, type);
                            Window::instance().rendering_mutex.lock();
                        }
                    }


                }
                else if (type == SELECTION_TYPE_VERTEX)
                {
                    int vertex_id = mesh->to_vertex_id(picked_id - from) - 1;

                    renderer->m_selection_hover_pass.hover(m_render_data, id, type, vertex_id);

                    OpenVolumeMesh::VertexHandle vertex(vertex_id);
                    if (vertex.is_valid() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                    {
                        // Select element via Window class, to activate Callback function
                        // To avoid problems with the Callback functions, we unlock the mutex guard here and lock it again after the method is done
                        Window::instance().rendering_mutex.unlock();
                        Window::instance().select_element(id, vertex_id, type);
                        Window::instance().rendering_mutex.lock();
                    }

                    mesh->get_mvb()->reset_hover();
                }
                else if (type == SELECTION_TYPE_EDGE)
                {
                    int edge_id = mesh->to_edge_id(picked_id - from) - 1;

                    renderer->m_selection_hover_pass.hover(m_render_data, id, type, edge_id);

                    OpenVolumeMesh::EdgeHandle edge(edge_id);
                    if (edge.is_valid() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                    {
                        // Select element via Window class, to activate Callback function
                        // To avoid problems with the Callback functions, we unlock the mutex guard here and lock it again after the method is done
                        Window::instance().rendering_mutex.unlock();
                        Window::instance().select_element(id, edge_id, type);
                        Window::instance().rendering_mutex.lock();
                    }
                    mesh->get_mvb()->reset_hover();
                }

                break;
            }
        }
        if (ImGui::IsWindowFocused() && ImGui::IsMouseDoubleClicked(0))
        {
            if (face_id_mesh >= 0)
            {
                Window::instance().rendering_mutex.unlock();
                // Focus
                Window::instance().camera_focus_on(face_id_mesh, face_id, 0.4);
                Window::instance().rendering_mutex.lock();
            }
        }
        if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape)))
        {
            // TODO Escape Focus
        }

        auto active_mesh = Window::instance().get_focused_mesh_object();
        if ((!any_mesh_hovered && active_mesh != nullptr) || !ImGui::IsWindowHovered())
        {
            for (const auto& m: Window::instance().get_mesh_list())
            {
                m.second->get_mvb()->reset_hover();
            }
            renderer->m_selection_hover_pass.hover(m_render_data, -1, 0, 0);
        }

        for (const auto& m: Window::instance().get_mesh_list())
        {
            int mesh_id = m.first;
            if (mesh_id != hovered_mesh_id)
            {
                m.second->get_mvb()->reset_hover();
            }
        }
    }

    void MeshView::show()
    {
        render_debug_menu();

        auto padding = ImGui::GetStyle().WindowPadding;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});
        ImGui::Begin("Mesh");

        // handle the things related to our mesh rendering canvas
        handleResize();

        renderer->set_target_framebuffer(m_meshFrameBuffer, m_screen_quad_frameBuffer);
        renderer->render(&m_render_data);

        // store the current top left position, so we can draw text here later on top of our canvas
        auto topLeft = ImGui::GetCursorPos();
        topLeft.x += padding.x;
        topLeft.y += padding.y;

        // finally, add the framebuffer texture as an image to the imgui window
        ImGui::GetWindowDrawList()->AddImage
                (
                        reinterpret_cast<ImTextureID>(get_selected_texture()),
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

        // Show hovered element type and id

        if (GlobalViewerSettings::getInstance()->get_selection_activated())
        {
            std::string hovered_element_name =
                    m_hovered_element_type == 3 ? "Face" : (m_hovered_element_type == 1 ? "Vertex" :
                                                            (m_hovered_element_type == 2 ? "Edge" : "Cell"));
            hovered_element_name += " : ";
            hovered_element_name += std::to_string(m_hovered_element_id);

            ImGui::SetCursorPos({ImGui::GetCursorPos().x + padding.x, ImGui::GetCursorPos().y});
            ImGui::Text( "%s", hovered_element_name.c_str());
        }

        // display mesh loading percentage
        for (const auto& [id, mesh] : Window::instance().get_mesh_list())
        {
            auto mvb = mesh->get_mvb();
            if (mvb != nullptr && !mvb->is_loading_finished())
            {
                ImVec2 text_size = ImGui::CalcTextSize("Loading: %%");
                float middle_x = ImGui::GetContentRegionAvailWidth() / 2.0f - text_size.x / 2.0f;
                ImGui::SetCursorPos({middle_x, topLeft.y});
                ImGui::Text(
                        "%s",
                        std::string("Loading: " + std::to_string((int) mvb->get_loading_percentage()) + "%").c_str()
                );
                break;
            }
        }

        /*
        if (Window::instance().has_mesh() && Window::instance().get_active_mesh_obj() != nullptr &&  Window::instance().get_active_mesh_obj()->m_mesh != nullptr)
        {
            auto mesh = Window::instance().get_focused_mesh_object()->m_mesh;

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

    void MeshView::render_debug_menu()
    {
        if (ImGui::Begin("Debug"))
        {
            ImGui::Text("Viewport");
            if (ImGui::RadioButton("Final Image", m_viewport_texture == FINAL_IMAGE))
            {
                m_viewport_texture = FINAL_IMAGE;
            }
            if (ImGui::RadioButton("Selection", m_viewport_texture == SELECTION))
            {
                m_viewport_texture = SELECTION;
                GlobalViewerSettings::getInstance()->set_selection_activated(true);
            }
            if (ImGui::RadioButton("SSAO Pre", m_viewport_texture == SSAO_PRE))
            {
                m_viewport_texture = SSAO_PRE;
            }
            if (ImGui::RadioButton("SSAO Blur", m_viewport_texture == SSAO_BLUR))
            {
                m_viewport_texture = SSAO_BLUR;
            }
            if (ImGui::RadioButton("Transparency Accum", m_viewport_texture == TRANSPARENCY_ACCUM))
            {
                m_viewport_texture = TRANSPARENCY_ACCUM;
            }
            if (ImGui::RadioButton("Transparency Reveal", m_viewport_texture == TRANSPARENCY_REVEAL))
            {
                m_viewport_texture = TRANSPARENCY_REVEAL;
            }
            if (ImGui::RadioButton("Shadow Map", m_viewport_texture == SHADOW_MAP))
            {
                m_viewport_texture = SHADOW_MAP;
            }
            if (m_viewport_texture == SHADOW_MAP)
            {
                int max = GlobalViewerSettings::getInstance()->get_cascade_level();
                ImGui::SliderInt("Cascade Level", &m_shadow_map_cascade_level_debug, 0, max - 1);
            }
        }
        ImGui::End();
        renderer->m_selection_pass.set_debug_mode(m_viewport_texture == SELECTION);
    }

    unsigned int MeshView::get_selected_texture()
    {
        switch (m_viewport_texture)
        {
            case FINAL_IMAGE:
                return m_screen_quad_frameBuffer->get_texture(GL_COLOR_ATTACHMENT0);
            case SELECTION:
                return renderer->m_selectionFrameBuffer->get_texture(GL_COLOR_ATTACHMENT0);
            case SSAO_PRE:
                return renderer->m_ssao_pass->get_ssao_texture();
            case SSAO_BLUR:
                return renderer->m_ssao_pass->get_blur_texture();
            case TRANSPARENCY_ACCUM:
                return renderer->m_transparency_pass_wb->get_accum_texture();
            case TRANSPARENCY_REVEAL:
                return renderer->m_transparency_pass_wb->get_reveal_texture();
            case SHADOW_MAP:
                return renderer->m_shadow_pass->shadow_maps[m_shadow_map_cascade_level_debug];
        }
        return -1;
    }
}
