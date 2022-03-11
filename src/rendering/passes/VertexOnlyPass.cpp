
#include "VertexOnlyPass.h"
#include "glad/glad.h"
#include "../../mesh/MeshObject.h"
#include "../../Window.h"

namespace vOS
{
    VertexOnlyPass::VertexOnlyPass()
    {
        m_vertex_only_shader = Shader::vertex_only_shader();
    }

    void VertexOnlyPass::render(VertexArrayObject* vao, const RenderData& data, int mesh_id)
    {
        // Get MeshObject
        MeshObject *obj = Window::instance().get_mesh_obj(mesh_id);

        // GL Setup
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        // Transform Data
        glm::mat4 transform = data.camera.world * obj->get_data().get_transform();
        glm::mat4 view_transform = data.camera.view * transform;

        // Cell operations
        float cell_size = obj->get_data().m_cell_size;
        int peel_depth = obj->get_data().m_peel_level;
        float slice_depth = obj->get_data().m_slice_level;

        auto bb = obj->get_transformed_bb(view_transform);
        auto min = bb.first;
        auto max = bb.second;

        // View Operations
        glm::vec3 view_dir = -glm::normalize(data.camera.get_front());
        auto slice_direction = obj->get_slice_dir(view_transform, view_dir);

        glm::vec3 cam_pos(data.camera.view * glm::vec4(data.camera.position, 1.0));

        m_vertex_only_shader->bind();

        m_vertex_only_shader->set_uniform_mat4f("u_mesh_transform", transform);
        m_vertex_only_shader->set_uniform_mat4f("u_projection", data.camera.projection);
        m_vertex_only_shader->set_uniform_mat4f("u_view", data.camera.view);
        m_vertex_only_shader->set_uniform_vec3f("u_cam_pos", cam_pos);
        m_vertex_only_shader->set_uniform_int("u_selection_offset", obj->get_data().m_selection_offset);
        m_vertex_only_shader->set_uniform_float("u_cell_size", cell_size);
        m_vertex_only_shader->set_uniform_int("u_peel_depth", peel_depth);
        m_vertex_only_shader->set_uniform_float("u_slice_depth", slice_depth);
        m_vertex_only_shader->set_uniform_vec3f("u_min", min);
        m_vertex_only_shader->set_uniform_vec3f("u_max", max);
        m_vertex_only_shader->set_uniform_vec3f("u_slice_direction", slice_direction);
        m_vertex_only_shader->set_uniform_bool("u_slice_locked", obj->get_data().m_slice_locked);
        m_vertex_only_shader->set_uniform_float("u_average_cell_size", obj->get_mvb()->get_average_cell_size());
        m_vertex_only_shader->set_uniform_vec4f("u_color", obj->get_data().m_color.get_rgba());

        obj->get_mvb()->get_vertex_only_vao()->draw_instanced(obj->get_num_visible_vertices());

        m_vertex_only_shader->unbind();
    }
}
