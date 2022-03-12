
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
        m_pre_pass = new PrePass(width, height);
        m_shadow_pass = new ShadowMapPass(this, width * 2, height * 2);
        m_transparent_shadow_pass = new TransparentShadowMapPass(width, height);
        m_shadow_color_filter_pass = new ShadowColorFilterPass(this, width, height);
        m_mesh_pass = new MeshPass(this);
        m_ssao_pass = new SSAOPass(this, width, height);

        m_meshFrameBuffer = new FrameBufferObject(width, height, FrameBufferObject::RGBA_AND_DEPTH_MULTISAMPLE);
        m_selectionFrameBuffer = new FrameBufferObject(width / 2, height / 2, FrameBufferObject::RGBA_AND_DEPTH);
        m_screen_quad_frameBuffer = new FrameBufferObject(width, height, FrameBufferObject::RGBA_AND_DEPTH);
        m_pixel_buffer = new PixelBufferObject(2, width / 2, height / 2);

        m_transparency_pass_wb = new TransparencyPass_WB(this, width, height);
        m_transparency_pass_dp = new TransparencyPass_DP(this, width, height);

        // Set Camera Viewport Size
        m_render_data.camera.set_viewport_size(width, height);

//        // setup light including projection and view for shadow map
//        m_render_data.light.color = glm::vec3{1.0f, 1.0f, 1.0f};
//        m_render_data.light.world = glm::mat4(1.0f);
//        //m_render_data.light.position = m_render_data.camera.position + glm::normalize(view_dir) * 20.0f;
//        m_render_data.light.position = glm::vec3{10.0f, 10.0f, 10.0f};
//        m_render_data.light.projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
//
//        m_render_data.light.view = glm::lookAt(
//            m_render_data.light.position,
//            glm::vec3(0.0f, 0.0f, 0.0f),
//            glm::vec3(0.0f, 1.0f, 0.0f)
//        );


        num_passes = 0;

        dp_layer = 0;
    }

    MeshView::~MeshView()
    {
        delete m_meshFrameBuffer;
        delete m_screen_quad_frameBuffer;
        delete m_selectionFrameBuffer;
        delete m_pixel_buffer;
        delete m_pre_pass;
        delete m_mesh_pass;
        delete m_transparency_pass_wb;
        delete m_ssao_pass;
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
            m_transparency_pass_wb->resize_buffers(m_viewportPanelWidth, m_viewportPanelHeight);
            m_transparency_pass_dp->resize_buffers(m_viewportPanelWidth, m_viewportPanelHeight);
            m_pre_pass->resize_buffers(m_viewportPanelWidth, m_viewportPanelHeight);
            m_ssao_pass->resize_buffers(m_viewportPanelWidth, m_viewportPanelHeight);
            m_shadow_pass->resize_buffers(m_viewportPanelWidth * 2, m_viewportPanelHeight * 2);
            m_shadow_color_filter_pass->resize_buffers(m_viewportPanelWidth, m_viewportPanelHeight);
            m_transparent_shadow_pass->resize_buffers(m_viewportPanelWidth, m_viewportPanelHeight);
            m_selectionFrameBuffer->resize(m_viewportPanelWidth / 2, m_viewportPanelHeight / 2);
            delete m_pixel_buffer;
            m_pixel_buffer = new PixelBufferObject(2, m_viewportPanelWidth / 2, m_viewportPanelHeight / 2);

            m_render_data.camera.set_viewport_size(width, height);

        }
    }


    void MeshView::renderMesh(int mesh_id)
    {

        // Get Mesh
        MeshObject* obj = Window::instance().get_mesh_obj(mesh_id);
        if (obj == nullptr)
            return;

        MeshData& mesh_data = obj->get_data();

        if (!mesh_data.m_visible)
        {
            return;
        }


        obj->update_vertex_buffer();

        VertexArrayObject* vao = obj->get_vao();
        if (GlobalViewerSettings::getInstance()->m_get_current_rounding_active())
        {
            vao = obj->get_mvb()->get_vao_rounded();
        }

        // render all passes
        if (vao != nullptr)
        {
            m_mesh_pass->render(vao, m_render_data, mesh_id);
            //m_shape_pass.render(nullptr, m_render_data, mesh_id);
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

        auto export_framebuffer_ms = new FrameBufferObject(export_width, export_height,
                                                           FrameBufferObject::RGBA_AND_DEPTH_MULTISAMPLE);
        auto export_framebuffer = new FrameBufferObject(export_width, export_height, FrameBufferObject::RGBA_AND_DEPTH);

        m_pre_pass->resize_buffers(export_width, export_height);
        m_ssao_pass->resize_buffers(export_width, export_height);

        render_pre_pass();
        render_ssao_pass();

        export_framebuffer_ms->bind();

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto active_mesh = Window::instance().get_focused_mesh_object();
        if (active_mesh != nullptr)
        {
            for (const std::pair<int, MeshObject*> m: Window::instance().get_mesh_list())
            {
                auto mesh = m.second;
                if (!mesh->get_data().m_visible)
                {
                    continue;
                }

                mesh->update_vertex_buffer();

                // render all passes
                if (mesh->get_vao() != nullptr)
                {
                    m_mesh_pass->render(mesh->get_vao(), m_render_data, m.first);
                    m_shape_pass.render(nullptr, m_render_data, m.first);
                }
            }
        }

        glFlush();
        glFinish();

        export_framebuffer_ms->unbind();

        // restore the old width and height
        m_viewportPanelWidth = prev_width;
        m_viewportPanelHeight = prev_height;
        m_pre_pass->resize_buffers(m_viewportPanelWidth, m_viewportPanelHeight);
        m_ssao_pass->resize_buffers(m_viewportPanelWidth, m_viewportPanelHeight);

        // copy our multisampled framebuffer to the output framebuffer
        FrameBufferObject::copy(GL_COLOR_ATTACHMENT0, GL_COLOR_BUFFER_BIT, export_framebuffer_ms, export_framebuffer);

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
        ImVec2 mouse_pos_in_window = {
                ImGui::GetMousePos().x - ImGui::GetCursorScreenPos().x - ImGui::GetScrollX(),
                ImGui::GetMousePos().y - ImGui::GetCursorScreenPos().y - ImGui::GetScrollY()
        };
        int x = (int) mouse_pos_in_window.x / 2;
        int y = (int) (viewport[3] * 2 - (int) mouse_pos_in_window.y) / 2;

        GLubyte* data = m_pixel_buffer->start_read(x, y, 1, 1);

        if (data != nullptr)
        {
            // evaluate ID out of color
            int type = data[0] & 3;
            int id;
            if (m_selection_pass.is_debug_mode())
            {
                id = (data[0] + data[1] * 256 + data[2] * 256 * 256) >> 2;
            }
            else
            {
                id = (data[0] + data[1] * 256 + data[2] * 256 * 256 + data[3] * 256 * 256 * 256) >> 2;
            }
            querySelection(type, id);
        }

        m_pixel_buffer->finish_read();

        m_current_frame = (m_current_frame + 1) % m_frame_limit;
        if (m_current_frame == 0)
        {
            // we need to clear our framebuffer as well
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            for (const std::pair<int, MeshObject*> m: Window::instance().get_mesh_list())
            {
                auto mesh = m.second;
                m_selection_pass.render_mesh(mesh, m_render_data, m.first);
            }
        }
        m_selectionFrameBuffer->unbind();

        m_meshFrameBuffer->bind();
        for (const std::pair<int, MeshObject*> m: Window::instance().get_mesh_list())
        {
            m_selection_hover_pass.render(nullptr, m_render_data, m.first);
        }
        m_meshFrameBuffer->unbind();
    }

    void MeshView::render_pre_pass()
    {
        m_pre_pass->get_framebuffer()->bind();
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_pre_pass->clear_position_buffer(m_render_data);
        for (const std::pair<int, MeshObject*> m: Window::instance().get_mesh_list())
        {
            auto mesh = m.second;
            if (!mesh->get_data().m_visible)
            {
                continue;
            }
            mesh->update_vertex_buffer();
            VertexArrayObject* vao = mesh->get_vao();
            if (GlobalViewerSettings::getInstance()->m_get_current_rounding_active())
            {
                vao = mesh->get_mvb()->get_vao_rounded();
            }
            if (vao != nullptr)
            {
                m_pre_pass->render(vao, m_render_data, m.first);
            }
        }
        // we generate a mipmap for the position, this is used for ssao
        // this needs to happen every frame, since the fragment position values always change
        glBindTexture(GL_TEXTURE_2D, m_pre_pass->get_framebuffer()->get_position_texture());
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        m_pre_pass->get_framebuffer()->unbind();
    }

    void MeshView::render_shadow_map()
    {
        // render opaque shadow map
        m_shadow_pass->get_framebuffer()->bind();
        glClearColor(0.0, 0.0, 0.0, 0.0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for (const std::pair<int, MeshObject*> m: Window::instance().get_mesh_list())
        {
            auto mesh = m.second;
            if (!mesh->get_data().m_visible)
            {
                continue;
            }
            mesh->update_vertex_buffer();
            VertexArrayObject* vao = mesh->get_vao();
            if (GlobalViewerSettings::getInstance()->m_get_current_rounding_active())
            {
                vao = mesh->get_mvb()->get_vao_rounded();
            }
            if (vao != nullptr)
            {
                m_shadow_pass->render(vao, m_render_data, m.first);
            }
        }
        m_shadow_color_filter_pass->get_framebuffer()->unbind();

        // render transparent shadow map
        m_transparent_shadow_pass->get_framebuffer()->bind();
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for (const std::pair<int, MeshObject*> m: Window::instance().get_mesh_list())
        {
            auto mesh = m.second;
            if (!mesh->get_data().m_visible)
            {
                continue;
            }
            mesh->update_vertex_buffer();
            VertexArrayObject* vao = mesh->get_vao();
            if (GlobalViewerSettings::getInstance()->m_get_current_rounding_active())
            {
                vao = mesh->get_mvb()->get_vao_rounded();
            }
            if (vao != nullptr)
            {
                m_transparent_shadow_pass->render(vao, m_render_data, m.first);
            }
        }
        m_transparent_shadow_pass->get_framebuffer()->unbind();


        // calculate color filter for transparent shadows
        m_shadow_color_filter_pass->get_framebuffer()->bind();

        //glClearColor(1.0, 1.0, 1.0, 0.0 );
        glClearColor(0.0, 0.0, 0.0, 0.0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for (const std::pair<int, MeshObject*> m: Window::instance().get_mesh_list())
        {
            auto mesh = m.second;
            if (!mesh->get_data().m_visible)
            {
                continue;
            }
            mesh->update_vertex_buffer();
            VertexArrayObject* vao = mesh->get_vao();
            if (GlobalViewerSettings::getInstance()->m_get_current_rounding_active())
            {
                vao = mesh->get_mvb()->get_vao_rounded();
            }
            if (vao != nullptr)
            {
                m_shadow_color_filter_pass->render(vao, m_render_data, m.first);
            }
        }
        m_shadow_color_filter_pass->get_framebuffer()->unbind();
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    }

    void MeshView::render_ssao_pass()
    {
        m_ssao_pass->render(nullptr, m_render_data, -1);
    }

    void MeshView::render_transparency_wb()
    {
        m_transparency_pass_wb->bind_transparent_buffer();
        m_transparency_pass_wb->clear_framebuffer();
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunci(0, GL_ONE, GL_ONE);
        glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
        glBlendEquation(GL_FUNC_ADD);
        //glDisable(GL_CULL_FACE);

        for (const std::pair<int, MeshObject*> m: Window::instance().get_mesh_list())
        {
            auto mesh = m.second;

            MeshData& mesh_data = mesh->get_data();

            if (!mesh->get_data().m_visible)
            {
                continue;
            }

//            if(!m_zoom)
//            {
//                m_zoom_point = mesh->get_mesh_offset();
//            }
//            mesh_data.offset = m_zoom_point;

            mesh->update_vertex_buffer();

            VertexArrayObject* vao = mesh->get_vao();
            if (GlobalViewerSettings::getInstance()->m_get_current_rounding_active())
            {
                vao = mesh->get_mvb()->get_vao_rounded();
            }
            if (vao != nullptr)
            {
                m_transparency_pass_wb->render(vao, m_render_data, m.first);
            }
        }
        m_transparency_pass_wb->unbind_transparent_buffer();

        glDepthFunc(GL_ALWAYS);
        glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);


        m_meshFrameBuffer->bind();
        m_transparency_pass_wb->render_composition();
        m_meshFrameBuffer->unbind();
    }

    void MeshView::render_transparency_dp()
    {
        num_passes = GlobalViewerSettings::getInstance()->m_get_current_number_passes();
        for (int i = 0; i < num_passes; i++)
        {
            if (i % 2 == 0)
            {
                m_transparency_pass_dp->m_transparent_framebuffer0->bind();
                dp_layer = m_transparency_pass_dp->m_transparent_framebuffer0->get_texture(GL_COLOR_ATTACHMENT0);
            }
            else
            {
                m_transparency_pass_dp->m_transparent_framebuffer1->bind();
                dp_layer = m_transparency_pass_dp->m_transparent_framebuffer1->get_texture(GL_COLOR_ATTACHMENT0);

            }
            glClearDepth(0.0f);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

            // first render all meshes
            for (const std::pair<int, MeshObject*> m: Window::instance().get_mesh_list())
            {
                auto mesh = m.second;
                MeshData& mesh_data = mesh->get_data();
                if (!mesh->get_data().m_visible)
                {
                    continue;
                }

                mesh->update_vertex_buffer();
                VertexArrayObject* vao = mesh->get_vao();
                if (GlobalViewerSettings::getInstance()->m_get_current_rounding_active())
                {
                    vao = mesh->get_mvb()->get_vao_rounded();
                }

                if (vao != nullptr)
                {
                    m_transparency_pass_dp->render(vao, m_render_data, m.first, i);
                }
            }
            if (i % 2 == 0)
            {
                m_transparency_pass_dp->m_transparent_framebuffer0->unbind();
            }
            else
            {
                m_transparency_pass_dp->m_transparent_framebuffer1->unbind();
            }

            m_transparency_pass_dp->render_composition(i, num_passes);

        }


    }

    void MeshView::querySelection(int type, int picked_id)
    {
        // evaluate which in which mesh the color was selected
        bool any_mesh_hovered = false;

        // Remember face id in case of double click
        int face_id = 0;
        int face_id_mesh = -1;

        for (const auto& m: Window::instance().get_mesh_list())
        {
            auto mesh = m.second;
            int from = std::get<0>(mesh->selection_offset());
            int to = std::get<1>(mesh->selection_offset());

            if (picked_id >= from && picked_id <= to)
            {
                int face_id_mesh = m.first;
                m_hovered_element_id = picked_id;
                m_hovered_element_type = type;

                any_mesh_hovered = true;

                if (type == SELECTION_TYPE_FACE)
                {
                    int halfface_id = mesh->to_halfface_id(picked_id - from) - 1;
                    mesh->get_mvb()->hover_halfface(halfface_id);

                    if (GlobalViewerSettings::getInstance()->m_get_current_isolation_state())
                    {

                        auto mvb = mesh->get_mvb();
                        mvb->start_isolation();

                        OpenVolumeMesh::HalfFaceHandle halfface{halfface_id};
                        int face_id = OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>::face_handle(
                                halfface).idx();

                        m_selection_hover_pass.hover(m_render_data, m.first, type, face_id);

                        OpenVolumeMesh::FaceHandle face(face_id);
                        OpenVolumeMesh::HalfFaceHandle hf = mesh->m_mesh->halfface_handle(face, 0);

                        OpenVolumeMesh::CellHandle cell_handle = mesh->m_mesh->incident_cell(hf);
                        OpenVolumeMesh::CellPropertyT<bool> isolateProp = mesh->m_mesh->request_cell_property<bool>(
                                "IsolateProperty");

                        if (cell_handle.idx() == -1)
                        {
                            OpenVolumeMesh::HalfFaceHandle hf1 = mesh->m_mesh->halfface_handle(face, 1);

                            cell_handle = mesh->m_mesh->incident_cell(hf1);

                            //std::cout << "Zelle 2: " << cell_handle.idx();
                        }
                        else
                        {
                            if (isolateProp[cell_handle] == 0.0)
                            {
                                OpenVolumeMesh::HalfFaceHandle hf1 = mesh->m_mesh->halfface_handle(face, 1);

                                cell_handle = mesh->m_mesh->incident_cell(hf1);
                            }
                        }
                        if (cell_handle.is_valid() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                        {

                            isolateProp[cell_handle] = 1.0;

                            auto mvb = mesh->get_mvb();
                            mvb->update_isolate_buffer(cell_handle.idx(), 1.0f);
                        }

                    }

                    if (GlobalViewerSettings::getInstance()->m_get_current_selection_mode() == CELL ||
                        GlobalViewerSettings::getInstance()->m_get_current_digging_activated())
                    {
                        int halfface_id = mesh->to_halfface_id(picked_id - from) - 1;
                        OpenVolumeMesh::HalfFaceHandle halfface{halfface_id};
                        face_id = OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>::face_handle(halfface).idx();

                        m_selection_hover_pass.hover(m_render_data, m.first, type, face_id);

                        OpenVolumeMesh::FaceHandle face(face_id);
                        OpenVolumeMesh::HalfFaceHandle hf = mesh->m_mesh->halfface_handle(face, 0);

                        OpenVolumeMesh::CellHandle cell_handle = mesh->m_mesh->incident_cell(hf);
                        OpenVolumeMesh::CellPropertyT<bool> diggingProp = mesh->m_mesh->request_cell_property<bool>(
                                "DiggingProperty");

                        if (cell_handle.idx() == -1)
                        {
                            OpenVolumeMesh::HalfFaceHandle hf1 = mesh->m_mesh->halfface_handle(face, 1);

                            cell_handle = mesh->m_mesh->incident_cell(hf1);
                        }
                        else
                        {
                            if (diggingProp[cell_handle] == 0.0)
                            {
                                OpenVolumeMesh::HalfFaceHandle hf1 = mesh->m_mesh->halfface_handle(face, 1);

                                cell_handle = mesh->m_mesh->incident_cell(hf1);
                            }
                        }

                        if (GlobalViewerSettings::getInstance()->m_get_current_digging_activated())
                        {
                            if (cell_handle.is_valid() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                            {
                                diggingProp[cell_handle] = 0.0;

                                auto mvb = mesh->get_mvb();
                                mvb->update_digging_buffer(cell_handle.idx(), 0.0f);
                            }
                        }
                        else
                        {
                            // cell_handle beinhaltet cell
                            if (cell_handle.is_valid() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                            {
                                // Select element via Window class, to activate Callback function
                                // To avoid problems with the Callback functions, we unlock the mutex guard here and lock it again after the method is done
                                Window::instance().rendering_mutex.unlock();
                                Window::instance().select_element(m.first, cell_handle.idx(), 6);
                                Window::instance().rendering_mutex.lock();
                            }
                        }


                    }
                    else
                    {

                        // because of unsigned int as return value mesh.to_halfface_id(pickedID) returns the id + 1 and 0 means
                        // there is no valid ID (e.g when clicking background)
                        int halfface_id = mesh->to_halfface_id(picked_id - from) - 1;
                        OpenVolumeMesh::HalfFaceHandle halfface{halfface_id};
                        face_id = OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>::face_handle(halfface).idx();

                        //std::cout << "hovering face with id: " << face_id << std::endl;

                        m_selection_hover_pass.hover(m_render_data, m.first, type, face_id);

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


                }
                else if (type == SELECTION_TYPE_VERTEX)
                {
                    int vertex_id = mesh->to_vertex_id(picked_id - from) - 1;

                    m_selection_hover_pass.hover(m_render_data, m.first, type, vertex_id);

                    OpenVolumeMesh::VertexHandle vertex(vertex_id);
                    if (vertex.is_valid() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                    {
                        // Select element via Window class, to activate Callback function
                        // To avoid problems with the Callback functions, we unlock the mutex guard here and lock it again after the method is done
                        Window::instance().rendering_mutex.unlock();
                        Window::instance().select_element(m.first, vertex_id, type);
                        Window::instance().rendering_mutex.lock();
                    }

                    mesh->get_mvb()->reset_hover();
                }
                else if (type == SELECTION_TYPE_EDGE)
                {
                    int edge_id = mesh->to_edge_id(picked_id - from) - 1;

                    m_selection_hover_pass.hover(m_render_data, m.first, type, edge_id);

                    OpenVolumeMesh::EdgeHandle edge(edge_id);
                    if (edge.is_valid() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                    {
                        // Select element via Window class, to activate Callback function
                        // To avoid problems with the Callback functions, we unlock the mutex guard here and lock it again after the method is done
                        Window::instance().rendering_mutex.unlock();
                        Window::instance().select_element(m.first, edge_id, type);
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
                Window().instance().rendering_mutex.unlock();
                // Focus
                Window().instance().camera_focus_on(face_id_mesh, face_id, 0.4);
                Window().instance().rendering_mutex.lock();
            }
        }
        if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape)))
        {
            // TODO Escape Focus
        }

        auto active_mesh = Window::instance().get_focused_mesh_object();
        if (!any_mesh_hovered && active_mesh != nullptr)
        {
            for (const auto& m: Window::instance().get_mesh_list())
            {
                m.second->get_mvb()->reset_hover();
            }
            m_selection_hover_pass.hover(m_render_data, 0, 0, 0);
        }
    }


    void MeshView::render_background()
    {

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        m_meshFrameBuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_background_pass.render(nullptr, m_render_data, 0);
        m_meshFrameBuffer->unbind();
    }


    void MeshView::render_meshes()
    {
        m_meshFrameBuffer->bind();
        for (const auto& m: Window::instance().get_mesh_list())
        {
            renderMesh(m.first);
        }
        m_meshFrameBuffer->unbind();
    }


    void MeshView::render_transparency()
    {
        m_transparency = GlobalViewerSettings::getInstance()->m_get_current_transparency_mode();

        switch (m_transparency)
        {
            case DEPTH_PEELING:
                render_transparency_dp();
                break;
            case WEIGHTED_BLENDED :
                render_transparency_wb();
        }
    }

    void MeshView::show()
    {
        render_debug_menu();

        m_render_data.rounding.active = GlobalViewerSettings::getInstance()->m_get_current_rounding_active();
        m_render_data.rounding.size = GlobalViewerSettings::getInstance()->m_get_current_rounding_size();

        auto padding = ImGui::GetStyle().WindowPadding;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});
        ImGui::Begin("Mesh");

        // handle the things related to our mesh rendering canvas
        handleResize();

        // Update Camera
        m_render_data.camera.update();

        // Render Meshes
        render_pre_pass();

        if (GlobalViewerSettings::getInstance()->m_get_current_mesh_mode() == ModeEnum::Only_Vertices)
        {
            render_background();
            m_meshFrameBuffer->bind();
            for (const std::pair<int, MeshObject*> m: Window::instance().get_mesh_list())
            {
                auto mesh = m.second;
                if (!mesh->get_data().m_visible)
                {
                    continue;
                }
                mesh->update_vertex_buffer();
                if (mesh->get_vao() != nullptr)
                {
                    m_vertex_only_pass.render(nullptr, m_render_data, m.first);
                }
            }
            m_meshFrameBuffer->unbind();
        }
        else
        {
            if (GlobalViewerSettings::getInstance()->m_get_current_ambient_occlusion_activated())
            {
                render_ssao_pass();
            }

            if (GlobalViewerSettings::getInstance()->m_get_current_shadows_activated())
            {
                render_shadow_map();
            }


            // Now render our mesh scene to the framebuffer texture
            // Start with opaque objects
            render_background();

            render_meshes();


            FrameBufferObject::copy(GL_DEPTH_ATTACHMENT, GL_DEPTH_BUFFER_BIT, m_meshFrameBuffer,
                                    m_screen_quad_frameBuffer);

            // Render transparent objects
            if (GlobalViewerSettings::getInstance()->m_get_current_transparency_activated())
            {
                render_transparency();
            }

            // Render Selection
            if (GlobalViewerSettings::getInstance()->m_get_current_selection_activated())
            {
                renderSelection();
            }
        }

        // set render states
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE); // enable depth writes so glClear won't ignore clearing the depth buffer
        glDisable(GL_BLEND);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // copy multisampled framebuffer that we rendered on to the imgui texture for display
        FrameBufferObject::copy(GL_COLOR_ATTACHMENT0, GL_COLOR_BUFFER_BIT, m_meshFrameBuffer,
                                m_screen_quad_frameBuffer);

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

        if (GlobalViewerSettings::getInstance()->m_get_current_selection_activated())
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
        for (const std::pair<int, MeshObject*> m: Window::instance().get_mesh_list())
        {
            auto mvb = m.second->get_mvb();
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
                GlobalViewerSettings::getInstance()->m_set_current_selection_activated(true);
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
        }
        ImGui::End();
        m_selection_pass.set_debug_mode(true);
    }

    unsigned int MeshView::get_selected_texture()
    {
        switch (m_viewport_texture)
        {
            case FINAL_IMAGE:
                return m_screen_quad_frameBuffer->get_texture(GL_COLOR_ATTACHMENT0);
            case SELECTION:
                return m_selectionFrameBuffer->get_texture(GL_COLOR_ATTACHMENT0);
            case SSAO_PRE:
                return m_ssao_pass->get_ssao_texture();
            case SSAO_BLUR:
                return m_ssao_pass->get_blur_texture();
            case TRANSPARENCY_ACCUM:
                return m_transparency_pass_wb->get_accum_texture();
            case TRANSPARENCY_REVEAL:
                return m_transparency_pass_wb->get_reveal_texture();
        }
        return -1;
    }


}
