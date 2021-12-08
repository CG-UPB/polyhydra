
#include "glad/glad.h"

#include "MeshView.h"
#include "../input/Input.h"
#include "LogWindow.h"
#include "../Window.h"

#include <algorithm>
#include <cmath>
#include <fstream>

#include "imgui.h"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/vec_swizzle.hpp"

#include "../mesh/MeshObject.h"
#include "../rendering/shapes/Box.h"

//#include "../util/BitMap.h"



namespace vOS
{
    const int BYTES_PER_PIXEL = 4; /// red, green, & blue
    const int FILE_HEADER_SIZE = 14;
    const int INFO_HEADER_SIZE = 40;
    MeshView::MeshView(int width, int height):
            m_viewportPanelWidth(width),
            m_viewportPanelHeight(height),
            m_lastDown(false),
            m_lastX(0.0),
            m_lastY(0.0),
            m_arcBallOn(false)
    {
        m_meshFrameBuffer = new FrameBufferObject(width, height);

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
                x/ (float) m_viewportPanelWidth * 2.0f - 1.0f,
                y / (float) m_viewportPanelHeight * 2.0f - 1.0f,
                0.0f
        );
        res.y = -res.y;
        float squared = res.x * res.x + res.y * res.y;
        if (squared <= 1.0f)
        {
            res.z = (float) sqrt(1.0 - squared);
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
                glm::mat3 camera_to_object = glm::inverse(glm::mat3(m_render_data.camera.view) * glm::mat3(m_render_data.camera.world));
                glm::vec3 axis_object = camera_to_object * axis_camera;
                m_render_data.camera.world = glm::rotate(m_render_data.camera.world, glm::degrees(angle) * speed, axis_object);
            }
        }
        m_lastX = mousePos.x;
        m_lastY = mousePos.y;
    }

    void MeshView::renderMesh()
    {
        int renderingMode = GlobalViewerSettings::getInstance()->m_get_current_rendering_mode();
        if(renderingMode == 0)
        {
            m_mesh_pass.set_wireframe_mode(true);
            m_mesh_pass.set_use_phong(false);
        }  
        else if(renderingMode == 1)
        {
            m_mesh_pass.set_wireframe_mode(false);
            m_mesh_pass.set_use_phong(true);
        }

        if(ImGui::IsKeyPressed(GLFW_KEY_W))
        {
            m_mesh_pass.set_wireframe_mode(!m_mesh_pass.get_wireframe_mode());
            m_mesh_pass.set_use_phong(!m_mesh_pass.get_use_phong());
        }
        // now render our mesh scene to the framebuffer texture
        m_meshFrameBuffer->bind();

        if(GlobalViewerSettings::getInstance()->m_get_color_activated())
        {
            float* color = GlobalViewerSettings::getInstance()->m_get_current_mesh_rendering_color();
            m_render_data.mesh.color = glm::vec3{color[0], color[1], color[2]};
        }
        
        // we need to clear our framebuffer as well
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto& mesh = Window::instance().get_mesh_obj();
        m_render_data.mesh.offset = mesh.get_mesh_offset();

        mesh.update_vertex_buffer();

        // render all passes
        m_background_pass.render(*mesh.get_vao(), m_render_data);
        if (mesh.get_vao() != nullptr)
        {
            m_mesh_pass.render(*mesh.get_vao(), m_render_data);
        }
        m_shape_pass.render(*mesh.get_vao(), m_render_data);
        if (mesh.get_vao() != nullptr)
        {
            m_highlight_pass.render(*mesh.get_vao(), m_render_data);
        }

        m_meshFrameBuffer->unbind();
    }

    unsigned char* createBitmapFileHeader (int height, int stride)
    {
        int fileSize = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (stride * height);

        static unsigned char fileHeader[] = {
                0,0,     /// signature
                0,0,0,0, /// image file size in bytes
                0,0,0,0, /// reserved
                0,0,0,0, /// start of pixel array
        };

        fileHeader[ 0] = (unsigned char)('B');
        fileHeader[ 1] = (unsigned char)('M');
        fileHeader[ 2] = (unsigned char)(fileSize      );
        fileHeader[ 3] = (unsigned char)(fileSize >>  8);
        fileHeader[ 4] = (unsigned char)(fileSize >> 16);
        fileHeader[ 5] = (unsigned char)(fileSize >> 24);
        fileHeader[10] = (unsigned char)(FILE_HEADER_SIZE + INFO_HEADER_SIZE);

        return fileHeader;
    }

    unsigned char* createBitmapInfoHeader (int height, int width)
    {
        static unsigned char infoHeader[] = {
                0,0,0,0, /// header size
                0,0,0,0, /// image width
                0,0,0,0, /// image height
                0,0,     /// number of color planes
                0,0,     /// bits per pixel
                0,0,0,0, /// compression
                0,0,0,0, /// image size
                0,0,0,0, /// horizontal resolution
                0,0,0,0, /// vertical resolution
                0,0,0,0, /// colors in color table
                0,0,0,0, /// important color count
        };

        infoHeader[ 0] = (unsigned char)(INFO_HEADER_SIZE);
        infoHeader[ 4] = (unsigned char)(width      );
        infoHeader[ 5] = (unsigned char)(width >>  8);
        infoHeader[ 6] = (unsigned char)(width >> 16);
        infoHeader[ 7] = (unsigned char)(width >> 24);
        infoHeader[ 8] = (unsigned char)(height      );
        infoHeader[ 9] = (unsigned char)(height >>  8);
        infoHeader[10] = (unsigned char)(height >> 16);
        infoHeader[11] = (unsigned char)(height >> 24);
        infoHeader[12] = (unsigned char)(1);
        infoHeader[14] = (unsigned char)(BYTES_PER_PIXEL*8);

        return infoHeader;
    }

    void generateBitmapImage (unsigned char* image, int height, int width, char* imageFileName)
    {
        int widthInBytes = width * BYTES_PER_PIXEL;

        unsigned char padding[3] = {0, 0, 0};
        int paddingSize = (4 - (widthInBytes) % 4) % 4;

        int stride = (widthInBytes) + paddingSize;

        FILE* imageFile = fopen(imageFileName, "wb");

        unsigned char* fileHeader = createBitmapFileHeader(height, stride);
        fwrite(fileHeader, 1, FILE_HEADER_SIZE, imageFile);

        unsigned char* infoHeader = createBitmapInfoHeader(height, width);
        fwrite(infoHeader, 1, INFO_HEADER_SIZE, imageFile);

        int i;
        for (i = 0; i < height; i++) {
            fwrite(image + (i*widthInBytes), BYTES_PER_PIXEL, width, imageFile);
            fwrite(padding, 1, paddingSize, imageFile);
        }

        fclose(imageFile);
    }

    void MeshView::m_take_screenshot(std::string filename)
    {
        m_meshFrameBuffer->bind();
        std::ofstream ofp;

        glFlush();
        glFinish();

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        ImVec2 screen_pos = ImGui::GetCursorScreenPos();
        int swidth = viewport[2];
        int sheight = viewport[3];
        unsigned char sdata[4*swidth*sheight];
        //unsigned char data[4*viewport[2]*viewport[3]];
        
        glReadPixels(0,0,swidth,sheight,GL_BGRA,GL_UNSIGNED_BYTE, sdata);

        int n = filename.length();

        // declaring character array
        char char_array[n + 1];

        // copying the contents of the
        // string to char array
        strcpy(char_array, filename.c_str());

        generateBitmapImage(sdata,sheight,swidth,char_array);

        m_meshFrameBuffer->unbind();

    }


    void MeshView::show()
    {
        if(GlobalViewerSettings::getInstance()->m_get_take_snapshot())
        {
            GlobalViewerSettings::getInstance()->m_set_take_snapshot(false);
            // Snapshot erstellen
            std::string filename = GlobalViewerSettings::getInstance()->m_get_actual_snapshot_filename();
            m_meshFrameBuffer->bind();
        std::ofstream ofp;

        glFlush();
        glFinish();

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        ImVec2 screen_pos = ImGui::GetCursorScreenPos();
        int swidth = viewport[2];
        int sheight = viewport[3];
        unsigned char sdata[4*swidth*sheight];
        //unsigned char data[4*viewport[2]*viewport[3]];
        
        glReadPixels(0,0,swidth,sheight,GL_BGRA,GL_UNSIGNED_BYTE, sdata);

        int n = filename.length();

        // declaring character array
        char char_array[n + 1];

        // copying the contents of the
        // string to char array
        strcpy(char_array, filename.c_str());

        generateBitmapImage(sdata,sheight,swidth,char_array);

        m_meshFrameBuffer->unbind();
        }


        auto padding = ImGui::GetStyle().WindowPadding;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2 {0.0f, 0.0f});
        ImGui::Begin("Mesh");

        // handle the things related to our mesh rendering canvas
        handleResize();
        handleMouseControl();
        renderMesh();

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

        //m_actual_snapshot = m_meshFrameBuffer->get_texture_id();
        

        // show frame time and fps
        ImGui::SetCursorPos(topLeft);
        ImGui::Text("%.3f ms", 1000.0f / ImGui::GetIO().Framerate);
        ImGui::SetCursorPos({ImGui::GetCursorPos().x + padding.x, ImGui::GetCursorPos().y});
        ImGui::Text("%.1f fps", ImGui::GetIO().Framerate);

        if (Window::instance().get_mesh_obj().m_mesh != nullptr)
        {
            ImGui::SetCursorPos({ImGui::GetCursorPos().x + padding.x, ImGui::GetCursorPos().y});
            ImGui::Text("vertices: %zu", Window::instance().get_mesh_obj().m_mesh->n_vertices());
            ImGui::SetCursorPos({ImGui::GetCursorPos().x + padding.x, ImGui::GetCursorPos().y});
            ImGui::Text("edges: %zu", Window::instance().get_mesh_obj().m_mesh->n_edges());
            ImGui::SetCursorPos({ImGui::GetCursorPos().x + padding.x, ImGui::GetCursorPos().y});
            ImGui::Text("faces: %zu", Window::instance().get_mesh_obj().m_mesh->n_faces());
        }

        ImGui::End();
        ImGui::PopStyleVar();
    }
}
