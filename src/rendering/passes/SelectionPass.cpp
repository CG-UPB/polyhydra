#include "SelectionPass.h"
#include "glad/glad.h"

#include "MeshPass.h"
#include "../meshes/CommonMeshes.h"
#include "../../settings/GlobalViewerSettings.h"
#include "../../Window.h"

namespace vOS {
    SelectionPass::SelectionPass(): m_selection_shader(Shader::selection_face())
    {
        m_selection_shader = Shader::selection_face();
        m_selection_sphere_shader = Shader::selection_vertex_shader();
        m_selection_cylinder_shader = Shader::selection_edge_shader();
    }

    void SelectionPass::render(VertexArrayObject* vao, const RenderData &data, int mesh_id)
    {
        // Get Mesh
        MeshObject *obj = Window::instance().get_mesh_obj(mesh_id);
        if (obj == nullptr)
            return;

        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        glm::mat4 positionOffset = glm::translate(-obj->get_data().offset);
        glm::mat4 transform = data.camera.world * obj->get_data().get_transform() * positionOffset;

        float cell_size = Window::instance().get_mesh_cell_size(mesh_id);
        int peel_depth = Window::instance().get_mesh_peel_level(mesh_id);
        float slice_depth = Window::instance().get_mesh_slice_level(mesh_id);
        auto bb = obj->get_transformed_bb(transform);
        auto min = bb.first;
        auto max = bb.second;

        glm::mat4 view_inv = glm::inverse(data.camera.view);
        glm::vec3 view_dir = {view_inv[2][0], view_inv[2][1], view_inv[2][2]};
        auto slice_direction = obj->get_slice_dir(transform, view_dir);

        int selection_mode = GlobalViewerSettings::getInstance()->m_get_current_selection_mode();

        bool faces_selectable = false;

        if(selection_mode == 0 || selection_mode == 3)
        {
            faces_selectable = true;
        }

        // draw faces
        m_selection_shader->bind();

        m_selection_shader->set_uniform_mat4f("u_mesh_transform", transform);
        m_selection_shader->set_uniform_mat4f("u_projection", data.camera.projection);
        m_selection_shader->set_uniform_mat4f("u_view", data.camera.view);
        m_selection_shader->set_uniform_int("u_selection_offset", obj->get_data().selection_offset);
        m_selection_shader->set_uniform_bool("u_debug_mode", DEBUG_MODE);
        m_selection_shader->set_uniform_bool("u_faces_selectable", faces_selectable);
        m_selection_shader->set_uniform_float("u_cell_size", cell_size);
        m_selection_shader->set_uniform_int("u_peel_depth", peel_depth);
        m_selection_shader->set_uniform_float("u_slice_depth", slice_depth);
        m_selection_shader->set_uniform_vec3f("u_min", min);
        m_selection_shader->set_uniform_vec3f("u_max", max);
        m_selection_shader->set_uniform_vec3f("u_slice_direction", slice_direction);
        m_selection_shader->set_uniform_bool("u_slice_locked", obj->get_data().m_slice_locked);

        vao->draw();

        m_selection_shader->unbind();

        // draw cylinders for each edge
        if(selection_mode == 0 || selection_mode == 2)
        {
            glDisable(GL_CULL_FACE);
            glDepthMask(GL_FALSE);

            m_selection_cylinder_shader->bind();

            m_selection_cylinder_shader->set_uniform_mat4f("u_mesh_transform", transform);
            m_selection_cylinder_shader->set_uniform_mat4f("u_projection", data.camera.projection);
            m_selection_cylinder_shader->set_uniform_mat4f("u_view", data.camera.view);
            m_selection_cylinder_shader->set_uniform_int("u_selection_offset", obj->get_data().selection_offset);
            m_selection_cylinder_shader->set_uniform_bool("u_debug_mode", DEBUG_MODE);
            m_selection_cylinder_shader->set_uniform_float("u_cell_size", cell_size);
            m_selection_cylinder_shader->set_uniform_int("u_peel_depth", peel_depth);
            m_selection_cylinder_shader->set_uniform_float("u_slice_depth", slice_depth);
            m_selection_cylinder_shader->set_uniform_vec3f("u_min", min);
            m_selection_cylinder_shader->set_uniform_vec3f("u_max", max);
            m_selection_cylinder_shader->set_uniform_vec3f("u_slice_direction", slice_direction);
            m_selection_cylinder_shader->set_uniform_bool("u_slice_locked", obj->get_data().m_slice_locked);

            m_cylinder_vao->draw_instanced(m_num_edges);

            m_selection_cylinder_shader->unbind();
        }

        glDepthMask(GL_TRUE);

        if (selection_mode == 0 || selection_mode == 1)
        {

            // draw spheres for each vertex
            m_selection_sphere_shader->bind();

            m_selection_sphere_shader->set_uniform_mat4f("u_mesh_transform", transform);
            m_selection_sphere_shader->set_uniform_mat4f("u_projection", data.camera.projection);
            m_selection_sphere_shader->set_uniform_mat4f("u_view", data.camera.view);
            m_selection_sphere_shader->set_uniform_vec3f("u_cam_pos", data.camera.position);
            m_selection_sphere_shader->set_uniform_int("u_selection_offset", obj->get_data().selection_offset);
            m_selection_sphere_shader->set_uniform_bool("u_debug_mode", DEBUG_MODE);
            m_selection_sphere_shader->set_uniform_float("u_cell_size", cell_size);
            m_selection_sphere_shader->set_uniform_int("u_peel_depth", peel_depth);
            m_selection_sphere_shader->set_uniform_float("u_slice_depth", slice_depth);
            m_selection_sphere_shader->set_uniform_vec3f("u_min", min);
            m_selection_sphere_shader->set_uniform_vec3f("u_max", max);
            m_selection_sphere_shader->set_uniform_vec3f("u_slice_direction", slice_direction);
            m_selection_sphere_shader->set_uniform_bool("u_slice_locked", obj->get_data().m_slice_locked);

            m_sphere_vao->draw_instanced(m_num_vertices);

            m_selection_sphere_shader->unbind();
        }
    }

    void SelectionPass::render_mesh(MeshObject* mesh, RenderData& data, int mesh_id)
    {
        // Get Mesh
        MeshObject *obj = Window::instance().get_mesh_obj(mesh_id);
        if (obj == nullptr)
            return;

        if (mesh != nullptr && mesh->get_vao() != nullptr)
        {
            int offset = std::get<0>(mesh->selection_offset());
            obj->get_data().selection_offset = offset;
            m_sphere_vao = mesh->get_sphere_vao();
            m_num_vertices = mesh->get_num_visible_vertices();
            m_cylinder_vao = mesh->get_cylinder_vao();
            m_num_edges = mesh->get_num_visible_edges();
            render(mesh->get_vao(), data, mesh_id);
        }
    }
}