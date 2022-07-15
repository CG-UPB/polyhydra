
#include "SelectionPass.h"
#include "MeshPass.h"
#include "../meshes/CommonMeshes.h"
#include "../Renderer.h"
#include "../gl/Shader.h"

namespace volumeshOS::Internal
{
    SelectionPass::SelectionPass()
    {
        // Get Shaders
        m_selection_shader = Shader::selection_face();
        m_selection_sphere_shader = Shader::selection_vertex_shader();
        m_selection_cylinder_shader = Shader::selection_edge_shader();
    }

    void SelectionPass::render(const Renderer& renderer)
    {
        // now render our mesh scene to the framebuffer texture
        renderer.buffers.selection_frame_buffer->bind();

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

        GLubyte* data = renderer.buffers.pixel_buffer->start_read(x, y, 1, 1);

        if (data != nullptr)
        {
            // evaluate ID out of color
            int type = data[0] & 3;
            int id;
            if (renderer.passes.selection_pass->is_debug_mode())
            {
                id = (data[0] + data[1] * 256 + data[2] * 256 * 256) >> 2;
            }
            else
            {
                id = (data[0] + data[1] * 256 + data[2] * 256 * 256 + data[3] * 256 * 256 * 256) >> 2;
            }
            renderer.selection_callback(type, id);
        }

        renderer.buffers.pixel_buffer->finish_read();
        if (renderer.frame.current == 0)
        {
            // we need to clear our framebuffer as well
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            for (const auto& mesh : renderer.render_list)
            {
                render_mesh(mesh, renderer);
            }
        }
        renderer.buffers.selection_frame_buffer->unbind();
    }

    void SelectionPass::render_mesh(const std::shared_ptr<MeshObject>& mesh, const Renderer& renderer)
    {
        // Set Variables from Mesh Data
        int offset = std::get<0>(mesh->selection_offset());
        mesh->get_data().selection_id_offset = offset;
        m_sphere_vao = mesh->get_sphere_vao();
        m_num_vertices = mesh->get_num_visible_vertices();
        m_cylinder_vao = mesh->get_cylinder_vao();
        m_num_edges = mesh->get_num_visible_edges();

        // GL Setup
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        // Transform Data
        glm::mat4 transform = renderer.camera->world * mesh->get_data().get_transform();
        glm::mat4 view_transform = renderer.camera->view * transform;

        // Cell operations
        float cell_size = mesh->get_data().cell_size;
        int peel_depth = mesh->get_data().peel_level;
        float slice_depth = mesh->get_data().slice_level;

        auto bb = mesh->get_world_bb(view_transform);
        auto min = bb.first;
        auto max = bb.second;

        // volumeshOS Operations
        glm::vec3 view_dir = -glm::normalize(renderer.camera->get_front());
        auto slice_direction = mesh->get_slice_dir(view_transform, view_dir);

        // Get SelectionMode Mode
        // 0 = Faces, 1 = Vertex, 2 = Edges, 3 = All
        auto selection_mode = AppState::settings.selection_mode;


        // Faces should not be selectable in Vertex or Edge SelectionMode mode
        bool faces_selectable = false;

        if(selection_mode == SelectionMode::ALL || selection_mode == SelectionMode::FACE || selection_mode == SelectionMode::CELL)
        {
            faces_selectable = true;
        }

        // Draw Faces
        m_selection_shader->bind();

        // Set Uniforms
        m_selection_shader->set_uniform_mat4f("u_mesh_transform", transform);
        m_selection_shader->set_uniform_mat4f("u_projection", renderer.camera->projection);
        m_selection_shader->set_uniform_mat4f("u_view", renderer.camera->view);
        m_selection_shader->set_uniform_int("u_selection_offset", mesh->get_data().selection_id_offset);
        m_selection_shader->set_uniform_bool("u_debug_mode", m_debug);
        m_selection_shader->set_uniform_bool("u_faces_selectable", faces_selectable);
        m_selection_shader->set_uniform_float("u_cell_size", cell_size);
        m_selection_shader->set_uniform_int("u_peel_depth", peel_depth);
        m_selection_shader->set_uniform_float("u_slice_depth", slice_depth);
        m_selection_shader->set_uniform_vec3f("u_min", min);
        m_selection_shader->set_uniform_vec3f("u_max", max);
        m_selection_shader->set_uniform_vec3f("u_slice_direction", slice_direction);
        m_selection_shader->set_uniform_bool("u_slice_locked", mesh->get_data().slice_locked);

        mesh->get_vao()->draw();

        m_selection_shader->unbind();

        if(selection_mode == SelectionMode::ALL || selection_mode == SelectionMode::EDGE)
        {
            // Draw cylinders for each Edge
            glDisable(GL_CULL_FACE);
            //glDepthMask(GL_FALSE);

            m_selection_cylinder_shader->bind();

            // Set Uniforms
            m_selection_cylinder_shader->set_uniform_mat4f("u_mesh_transform", transform);
            m_selection_cylinder_shader->set_uniform_mat4f("u_projection", renderer.camera->projection);
            m_selection_cylinder_shader->set_uniform_mat4f("u_view", renderer.camera->view);
            m_selection_cylinder_shader->set_uniform_int("u_selection_offset", mesh->get_data().selection_id_offset);
            m_selection_cylinder_shader->set_uniform_bool("u_debug_mode", m_debug);
            m_selection_cylinder_shader->set_uniform_float("u_cell_size", cell_size);
            m_selection_cylinder_shader->set_uniform_int("u_peel_depth", peel_depth);
            m_selection_cylinder_shader->set_uniform_float("u_slice_depth", slice_depth);
            m_selection_cylinder_shader->set_uniform_vec3f("u_min", min);
            m_selection_cylinder_shader->set_uniform_vec3f("u_max", max);
            m_selection_cylinder_shader->set_uniform_vec3f("u_slice_direction", slice_direction);
            m_selection_cylinder_shader->set_uniform_bool("u_slice_locked", mesh->get_data().slice_locked);
            m_selection_cylinder_shader->set_uniform_float("u_average_cell_size", mesh->get_mvb()->get_average_cell_size());

            m_cylinder_vao->draw_instanced(m_num_edges);

            m_selection_cylinder_shader->unbind();
        }

        glDepthMask(GL_TRUE);

        if (selection_mode == SelectionMode::ALL || selection_mode == SelectionMode::VERTEX)
        {

            // Draw spheres for each Vertex
            m_selection_sphere_shader->bind();

            // Set Uniforms
            m_selection_sphere_shader->set_uniform_mat4f("u_mesh_transform", transform);
            m_selection_sphere_shader->set_uniform_mat4f("u_projection", renderer.camera->projection);
            m_selection_sphere_shader->set_uniform_mat4f("u_view", renderer.camera->view);
            m_selection_sphere_shader->set_uniform_vec3f("u_cam_pos", renderer.camera->position);
            m_selection_sphere_shader->set_uniform_int("u_selection_offset", mesh->get_data().selection_id_offset);
            m_selection_sphere_shader->set_uniform_bool("u_debug_mode", m_debug);
            m_selection_sphere_shader->set_uniform_float("u_cell_size", cell_size);
            m_selection_sphere_shader->set_uniform_int("u_peel_depth", peel_depth);
            m_selection_sphere_shader->set_uniform_float("u_slice_depth", slice_depth);
            m_selection_sphere_shader->set_uniform_vec3f("u_min", min);
            m_selection_sphere_shader->set_uniform_vec3f("u_max", max);
            m_selection_sphere_shader->set_uniform_vec3f("u_slice_direction", slice_direction);
            m_selection_sphere_shader->set_uniform_bool("u_slice_locked", mesh->get_data().slice_locked);
            m_selection_sphere_shader->set_uniform_float("u_average_cell_size", mesh->get_mvb()->get_average_cell_size());



            m_sphere_vao->draw_instanced(m_num_vertices);

            m_selection_sphere_shader->unbind();
        }
    }

    void SelectionPass::set_debug_mode(bool mode)
    {
        m_debug = mode;
    }

    bool SelectionPass::is_debug_mode() const
    {
        return m_debug;
    }
}