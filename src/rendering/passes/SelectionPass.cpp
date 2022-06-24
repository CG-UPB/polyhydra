
#include "SelectionPass.h"
#include "MeshPass.h"
#include "../meshes/CommonMeshes.h"

namespace volumeshOS {
    SelectionPass::SelectionPass()
    {
        // Get Shaders
        m_selection_shader = Shader::selection_face();
        m_selection_sphere_shader = Shader::selection_vertex_shader();
        m_selection_cylinder_shader = Shader::selection_edge_shader();
    }

    void SelectionPass::render(std::shared_ptr<VertexArrayObject> vao, const RenderData &data, std::shared_ptr<MeshObject> mesh)
    {
        // GL Setup
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        // Transform Data
        glm::mat4 transform = data.camera.world * mesh->get_data().get_transform();
        glm::mat4 view_transform = data.camera.view * transform;

        // Cell operations
        float cell_size = mesh->get_data().cell_size;
        int peel_depth = mesh->get_data().peel_level;
        float slice_depth = mesh->get_data().slice_level;

        auto bb = mesh->get_world_bb(view_transform);
        auto min = bb.first;
        auto max = bb.second;

        // volumeshOS Operations
        glm::vec3 view_dir = -glm::normalize(data.camera.get_front());
        auto slice_direction = mesh->get_slice_dir(view_transform, view_dir);

        // Get Selection Mode
        // 0 = Faces, 1 = Vertex, 2 = Edges, 3 = All
        int selection_mode = GlobalViewerSettings::getInstance()->get_selection_mode();


        // Faces should not be selectable in Vertex or Edge Selection mode
        bool faces_selectable = false;

        if(selection_mode == Selection::ALL || selection_mode == Selection::Face || selection_mode == Selection::CELL)
        {
            faces_selectable = true;
        }

        // Draw Faces
        m_selection_shader->bind();

        // Set Uniforms
        m_selection_shader->set_uniform_mat4f("u_mesh_transform", transform);
        m_selection_shader->set_uniform_mat4f("u_projection", data.camera.projection);
        m_selection_shader->set_uniform_mat4f("u_view", data.camera.view);
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

        vao->draw();

        m_selection_shader->unbind();

        if(selection_mode == Selection::ALL || selection_mode == Selection::Edge)
        {
            // Draw cylinders for each Edge
            glDisable(GL_CULL_FACE);
            //glDepthMask(GL_FALSE);

            m_selection_cylinder_shader->bind();

            // Set Uniforms
            m_selection_cylinder_shader->set_uniform_mat4f("u_mesh_transform", transform);
            m_selection_cylinder_shader->set_uniform_mat4f("u_projection", data.camera.projection);
            m_selection_cylinder_shader->set_uniform_mat4f("u_view", data.camera.view);
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

        if (selection_mode == Selection::ALL || selection_mode == Selection::Vertex)
        {

            // Draw spheres for each Vertex
            m_selection_sphere_shader->bind();

            // Set Uniforms
            m_selection_sphere_shader->set_uniform_mat4f("u_mesh_transform", transform);
            m_selection_sphere_shader->set_uniform_mat4f("u_projection", data.camera.projection);
            m_selection_sphere_shader->set_uniform_mat4f("u_view", data.camera.view);
            m_selection_sphere_shader->set_uniform_vec3f("u_cam_pos", data.camera.position);
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

    void SelectionPass::render_mesh(std::shared_ptr<MeshObject> const& mesh, RenderData& data)
    {
        if (mesh != nullptr && mesh->get_vao() != nullptr)
        {
            // Set Variables from Mesh Data
            int offset = std::get<0>(mesh->selection_offset());
            mesh->get_data().selection_id_offset = offset;
            m_sphere_vao = mesh->get_sphere_vao();
            m_num_vertices = mesh->get_num_visible_vertices();
            m_cylinder_vao = mesh->get_cylinder_vao();
            m_num_edges = mesh->get_num_visible_edges();
            render(mesh->get_vao(), data, mesh);
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