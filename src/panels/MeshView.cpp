
#include "MeshView.h"
#include "../util/StringUtil.h"
#include "rendering/Renderer.h"
#include "../util/ImGuiUtil.h"

namespace volumeshOS::Internal
{
    MeshView::MeshView(int width, int height) :
            m_viewport_panel_width(width),
            m_viewport_panel_height(height)
    {
        renderer = std::make_shared<Renderer>(width, height);
        renderer->set_selection_callback([&](int type, int id){
            handle_mouse_hover(type, id);
        });
        log_window = std::make_shared<LogWindow>();

    }

    void MeshView::handle_resize()
    {
        // if our window panel size changes, we need to adjust the framebuffer size and projection
        auto viewPortPanelSize = ImGui::GetContentRegionAvail();
        float width = std::max(viewPortPanelSize.x, 100.0f);
        float height = std::max(viewPortPanelSize.y, 100.0f);
        if (width != (float) m_viewport_panel_width || height != (float) m_viewport_panel_height)
        {
            m_viewport_panel_width = (int) width;
            m_viewport_panel_height = (int) height;
            renderer->resize(m_viewport_panel_width, m_viewport_panel_height);
        }
    }

    void MeshView::handle_vertex_hover(const std::shared_ptr<MeshObject>& mesh, OpenVolumeMesh::VertexHandle vertex)
    {
        renderer->passes.selection_hover_pass->hover(mesh, SELECTION_TYPE_VERTEX, vertex.idx());
        mesh->get_mvb()->reset_hover();
        auto& callbacks = AppState::callbacks;
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            callbacks.on_vertex_select(VMesh(mesh->get_id()), vertex);
        }
        callbacks.on_vertex_hover(VMesh(mesh->get_id()), vertex);
        m_hovered_element_type = SELECTION_TYPE_VERTEX;
        m_hovered_element_ovm_id = vertex.idx();
    }

    void MeshView::handle_edge_hover(const std::shared_ptr<MeshObject>& mesh, OpenVolumeMesh::EdgeHandle edge)
    {
        renderer->passes.selection_hover_pass->hover(mesh, SELECTION_TYPE_EDGE, edge.idx());
        mesh->get_mvb()->reset_hover();
        auto& callbacks = AppState::callbacks;
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            callbacks.on_edge_select(VMesh(mesh->get_id()), edge);
        }
        callbacks.on_edge_hover(VMesh(mesh->get_id()), edge);
        m_hovered_element_type = SELECTION_TYPE_EDGE;
        m_hovered_element_ovm_id = edge.idx();
    }

    void MeshView::handle_halfface_hover(const std::shared_ptr<MeshObject>& mesh, OpenVolumeMesh::HalfFaceHandle halfface)
    {
        auto& settings = AppState::settings;
        auto& callbacks = AppState::callbacks;
        if (settings.selection_mode == SelectionMode::CELL)
        {
            auto cell = mesh->get_ovm()->incident_cell(halfface);
            if (cell.is_valid())
            {
                mesh->get_mvb()->hover_cell(cell.idx());
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    // isolation should always be preferred over digging
                    if (settings.isolation_active)
                    {
                        mesh->get_mvb()->set_cell_isolated(cell.idx());
                        renderer->shapes->update_cell(cell.idx());
                    }
                    else if (settings.digging_active)
                    {
                        mesh->get_mvb()->set_cell_digged(cell.idx(), true);
                        renderer->shapes->update_cell(cell.idx());
                    }
                    callbacks.on_cell_select(VMesh(mesh->get_id()), cell);
                }
                callbacks.on_cell_hover(VMesh(mesh->get_id()), cell);
                m_hovered_element_type = SELECTION_TYPE_CELL;
                m_hovered_element_ovm_id = cell.idx();

                auto pos = mesh->get_ovm()->barycenter(cell);
                renderer->hover_position = to_glm_vec3(pos);
            }
        }
        else
        {
            mesh->get_mvb()->hover_halfface(halfface.idx());
            auto face = OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>::face_handle(halfface);
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                if (face.is_valid())
                {
                    callbacks.on_face_select(VMesh(mesh->get_id()), face);
                }
                callbacks.on_halfface_select(VMesh(mesh->get_id()), halfface);
            }
            if (face.is_valid())
            {
                callbacks.on_face_hover(VMesh(mesh->get_id()), face);
                auto pos = mesh->get_ovm()->barycenter(face);
                renderer->hover_position = to_glm_vec3(pos);
            }
            callbacks.on_halfface_hover(VMesh(mesh->get_id()), halfface);
            m_hovered_element_type = SELECTION_TYPE_HALFFACE;
            m_hovered_element_ovm_id = halfface.idx();
        }
        renderer->passes.selection_hover_pass->hover(mesh, SELECTION_TYPE_NONE, 0);
    }

    void MeshView::handle_mouse_hover(int type, int picked_id)
    {
        bool anything_hovered = false;
        for (const auto& mesh : renderer->render_list) {
            auto [from, to] = mesh->selection_offset();
            if (picked_id >= from && picked_id <= to)
            {
                anything_hovered = true;
                switch (type)
                {
                    case SELECTION_TYPE_VERTEX:
                    {
                        int id = mesh->to_vertex_id(picked_id - from) - 1;
                        auto vertex = OpenVolumeMesh::VertexHandle{id};
                        if (vertex.is_valid())
                        {
                            handle_vertex_hover(mesh, vertex);
                            auto pos = mesh->get_ovm()->vertex(vertex);
                            renderer->hover_position = to_glm_vec3(pos);
                        }
                        else
                        {
                            Log::warn("Invalid vertex handle hovered: " + std::to_string(id));
                        }
                        break;
                    }
                    case SELECTION_TYPE_EDGE:
                    {
                        int id = mesh->to_edge_id(picked_id - from) - 1;
                        auto edge = OpenVolumeMesh::EdgeHandle{id};
                        if (edge.is_valid())
                        {
                            handle_edge_hover(mesh, edge);
                            auto pos = mesh->get_ovm()->barycenter(edge);
                            renderer->hover_position = to_glm_vec3(pos);
                        }
                        else
                        {
                            Log::warn("Invalid edge handle hovered: " + std::to_string(id));
                        }
                        break;
                    }
                    case SELECTION_TYPE_HALFFACE:
                    {
                        int id = mesh->to_halfface_id(picked_id - from) - 1;
                        auto halfface = OpenVolumeMesh::HalfFaceHandle{id};
                        if (halfface.is_valid())
                        {
                            handle_halfface_hover(mesh, halfface);
                        }
                        else
                        {
                            Log::warn("Invalid halfface handle hovered: " + std::to_string(id));
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
            else
            {
                mesh->get_mvb()->reset_hover();
            }
            if (!ImGui::IsWindowHovered())
            {
                mesh->get_mvb()->reset_hover();
            }
        }
        if (!anything_hovered || !ImGui::IsWindowHovered())
        {
            auto mesh = renderer->mesh_list->get_focused_mesh();
            if(mesh != nullptr)
            {
                renderer->hover_position = mesh->get_data().position + mesh->get_data().position_offset;
            }
            else
            {
                renderer->hover_position = {0.0f, 0.0f, 0.0f};
            }

            renderer->passes.selection_hover_pass->hover(nullptr, SELECTION_TYPE_NONE, 0);
            m_hovered_element_type = SELECTION_TYPE_NONE;
            m_hovered_element_ovm_id = -1;
        }
    }

    void MeshView::show()
    {
        render_debug_menu();


        auto padding = ImGui::GetStyle().WindowPadding;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});
        ImGui::Begin("Mesh");

        // handle the things related to our mesh rendering canvas
        handle_resize();


        renderer->render();

        // store the current top left position, so we can draw text here later on top of our canvas
        auto topLeft = ImGui::GetCursorPos();
        topLeft.x += padding.x;
        topLeft.y += padding.y;

        // finally, add the framebuffer texture as an image to the imgui window
        ImGui::GetWindowDrawList()->AddImage
                (
                        reinterpret_cast<ImTextureID>(get_selected_texture()),
                        ImGui::GetCursorScreenPos(),
                        {ImGui::GetCursorScreenPos().x + (float) m_viewport_panel_width,
                         ImGui::GetCursorScreenPos().y + (float) m_viewport_panel_height},
                        {0.0f, 1.0f},
                        {1.0f, 0.0f}
                );

        // show frame time and fps
        ImGui::SetCursorPos(topLeft);
        ImGui::Text("%.3f ms", 1000.0f / ImGui::GetIO().Framerate);
        ImGui::SetCursorPos({ImGui::GetCursorPos().x + padding.x, ImGui::GetCursorPos().y});
        ImGui::Text("%.1f fps", ImGui::GetIO().Framerate);
        float y_pos = ImGui::GetCursorPosY();

        // display mesh loading percentage
        const auto mesh = renderer->mesh_list->get_focused_mesh();

        const float progress_bar_width = 150.0f;
        const float progress_bar_height = 30.0f;

        if(read_data)
        {
            read_data = false;
            auto text = "Reading file...";
            ImVec2 text_size = ImGui::CalcTextSize(text);
            float middle_x = ImGui::GetContentRegionAvailWidth() / 2.0f - text_size.x / 2.0f;
            ImGui::SetCursorPos({middle_x - text_size.x / 2.0f, topLeft.y});
            ImGui::Text("%s", text);
        }

        if (mesh != nullptr)
        {
            const auto mvb = mesh->get_mvb();

            if (mvb != nullptr && !mvb->is_loading_finished())
            {
                ImVec2 text_size = ImGui::CalcTextSize("Loading: %%");
                float middle_x = ImGui::GetContentRegionAvailWidth() / 2.0f - text_size.x / 2.0f;
                ImGui::SetCursorPos({middle_x - progress_bar_width / 2.0f, topLeft.y});
                ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
                ImGui::ProgressBar(mvb->get_loading_percentage() / 100.0f, ImVec2(progress_bar_width, progress_bar_height));
                ImGui::PopStyleColor();
            }
            ImGui::SetCursorPos({ImGui::GetCursorPos().x + padding.x, y_pos});
            ImGui::Text("vertices: %zu", mesh->get_ovm()->n_logical_vertices());
            ImGui::SetCursorPos({ImGui::GetCursorPos().x + padding.x, ImGui::GetCursorPos().y});
            ImGui::Text("edges: %zu",mesh->get_ovm()->n_logical_edges());
            ImGui::SetCursorPos({ImGui::GetCursorPos().x + padding.x, ImGui::GetCursorPos().y});
            ImGui::Text("faces: %zu", mesh->get_ovm()->n_logical_faces());
            ImGui::SetCursorPos({ImGui::GetCursorPos().x + padding.x, ImGui::GetCursorPos().y});
            ImGui::Text("cells: %zu", mesh->get_ovm()->n_logical_cells());
        }

        // Show hovered element type and id
        if (AppState::settings.selection_active && m_hovered_element_type != SELECTION_TYPE_NONE)
        {
            std::string element_name = SELECTION_TYPE_NAME[m_hovered_element_type];
            element_name += " : " + std::to_string(m_hovered_element_ovm_id);
            ImGui::SetCursorPos({ImGui::GetCursorPos().x + padding.x, ImGui::GetCursorPos().y});
            ImGui::Text( "%s", element_name.c_str());
        }

        // render LogWindow

        auto c = ImGui::GetContentRegionMax();
        c.y -= 8.0f;
        log_window->show(c.x, c.y, renderer);


        ImGui::End();
        ImGui::PopStyleVar();
    }

    void MeshView::render_debug_menu()
    {
        if (ImGui::Begin("Debug"))
        {
            auto& statistics = AppState::statistics;
            ImGuiUtil::push_bold_font();
            ImGui::Text("Statistics");
            ImGui::PopFont();
            float offset = 200.0f;
            ImGui::Text("Draw calls:");
            ImGui::SameLine(offset);
            ImGui::Text("%d", statistics.draw_calls_per_frame);
            ImGui::Text("Total drawn vertices:");
            ImGui::SameLine(offset);
            ImGui::Text("%d", statistics.total_rendered_vertices_per_frame);
            ImGui::Text("Total drawn triangles:");
            ImGui::SameLine(offset);
            ImGui::Text("%d", statistics.total_rendered_triangles_per_frame);

            ImGuiUtil::push_bold_font();
            ImGui::Text("Viewport");
            ImGui::PopFont();
            if (ImGui::RadioButton("Final Image", m_viewport_texture == FINAL_IMAGE))
            {
                m_viewport_texture = FINAL_IMAGE;
            }
            if (ImGui::RadioButton("Selection", m_viewport_texture == SELECTION))
            {
                m_viewport_texture = SELECTION;
                AppState::settings.selection_active = true;
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
                int max = AppState::settings.num_shadow_cascades;
                ImGui::SliderInt("Cascade Level", &m_shadow_map_cascade_level_debug, 0, max - 1);
            }
        }
        ImGui::End();
        renderer->passes.selection_pass->set_debug_mode(m_viewport_texture == SELECTION);
    }

    uint32_t MeshView::get_selected_texture()
    {
        switch (m_viewport_texture)
        {
            case FINAL_IMAGE:
                return renderer->buffers.post_framebuffer->get_texture(GL_COLOR_ATTACHMENT0);
            case SELECTION:
                return renderer->buffers.selection_frame_buffer->get_texture(GL_COLOR_ATTACHMENT0);
            case SSAO_PRE:
                return renderer->passes.ssao_pass->get_ssao_texture();
            case SSAO_BLUR:
                return renderer->passes.ssao_pass->get_blur_texture();
            case TRANSPARENCY_ACCUM:
                return renderer->passes.transparency_pass_wb->get_accum_texture();
            case TRANSPARENCY_REVEAL:
                return renderer->passes.transparency_pass_wb->get_reveal_texture();
            case SHADOW_MAP:
                return renderer->passes.shadow_pass->shadow_maps[m_shadow_map_cascade_level_debug];
        }
        return -1;
    }
}
