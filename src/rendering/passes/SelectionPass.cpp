#include "SelectionPass.h"
#include "glad/glad.h"

#include "MeshPass.h"
#include "../meshes/CommonMeshes.h"
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
        glm::mat4 transform = data.camera.world * obj->get_data().transform * positionOffset;

        // draw faces
        m_selection_shader->bind();

        m_selection_shader->set_uniform_mat4f("u_mesh_transform", transform);
        m_selection_shader->set_uniform_mat4f("u_projection", data.camera.projection);
        m_selection_shader->set_uniform_mat4f("u_view", data.camera.view);
        m_selection_shader->set_uniform_int("u_selection_offset", obj->get_data().selection_offset);
        m_selection_shader->set_uniform_bool("u_debug_mode", DEBUG_MODE);

        vao->draw();

        glDisable(GL_CULL_FACE);
        glDepthMask(GL_FALSE);

        // draw cylinders for each edge
        m_selection_cylinder_shader->bind();

        m_selection_cylinder_shader->set_uniform_mat4f("u_mesh_transform", transform);
        m_selection_cylinder_shader->set_uniform_mat4f("u_projection", data.camera.projection);
        m_selection_cylinder_shader->set_uniform_mat4f("u_view", data.camera.view);
        m_selection_cylinder_shader->set_uniform_int("u_selection_offset", obj->get_data().selection_offset);
        m_selection_cylinder_shader->set_uniform_bool("u_debug_mode", DEBUG_MODE);

        m_cylinder_vao->draw_instanced(m_num_edges);

        glDepthMask(GL_TRUE);

        // draw spheres for each vertex
        m_selection_sphere_shader->bind();

        //std::cout << "scale: " << glm::length(transform[0]) << std::endl;

        m_selection_sphere_shader->set_uniform_mat4f("u_mesh_transform", transform);
        m_selection_sphere_shader->set_uniform_mat4f("u_projection", data.camera.projection);
        m_selection_sphere_shader->set_uniform_mat4f("u_view", data.camera.view);
        m_selection_sphere_shader->set_uniform_vec3f("u_cam_pos", data.camera.position);
        m_selection_sphere_shader->set_uniform_int("u_selection_offset", obj->get_data().selection_offset);
        m_selection_sphere_shader->set_uniform_bool("u_debug_mode", DEBUG_MODE);

        m_sphere_vao->draw_instanced(m_num_vertices);

        m_selection_sphere_shader->unbind();
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
            auto d = obj->get_data();
            d.selection_offset = offset;
            obj->set_data((d));
            m_sphere_vao = mesh->get_sphere_vao();
            m_num_vertices = mesh->get_num_visible_vertices();
            m_cylinder_vao = mesh->get_cylinder_vao();
            m_num_edges = mesh->get_num_visible_edges();
            render(mesh->get_vao(), data, mesh_id);
        }
    }
}