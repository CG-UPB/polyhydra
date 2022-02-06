
#include "glad/glad.h"
#include "../../Window.h"
#include "PrePass.h"

namespace vOS
{
    void PrePass::render(VertexArrayObject* vao, const RenderData& data, int mesh_id)
    {
        // Get Mesh
        MeshObject* obj = Window::instance().get_mesh_obj(mesh_id);
        if(obj == nullptr)
            return;

        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        auto pre_phong_shader = Shader::pre_mesh_phong_shader();

        pre_phong_shader->bind();

        glm::mat4 positionOffset = glm::translate(-obj->get_data().offset);
        glm::mat4 transform = data.camera.world * obj->get_data().get_transform() * positionOffset;
        float cell_size = obj->get_data().m_cell_size;
        int peel_depth = obj->get_data().m_peel_level;
        float slice_depth = obj->get_data().m_slice_level;

        auto bb = obj->get_transformed_bb(transform);
        auto min = bb.first;
        auto max = bb.second;

        glm::mat4 view_inv = glm::inverse(data.camera.view);
        glm::vec3 view_dir = {view_inv[2][0], view_inv[2][1], view_inv[2][2]};
        auto slice_direction = obj->get_slice_dir(transform, view_dir);

        // set all of our uniforms
        pre_phong_shader->set_uniform_mat4f("u_Transform", transform);
        pre_phong_shader->set_uniform_mat4f("u_Projection", data.camera.projection);
        pre_phong_shader->set_uniform_mat4f("u_View", data.camera.view);
        pre_phong_shader->set_uniform_vec3f("u_lightPos", data.light.position);
        pre_phong_shader->set_uniform_vec3f("u_camPos", data.camera.position);
        pre_phong_shader->set_uniform_vec3f("u_lightColor", data.light.color);
        pre_phong_shader->set_uniform_float("u_cell_size", cell_size);
        pre_phong_shader->set_uniform_vec3f("u_objectColor", obj->get_data().m_color.get());
        pre_phong_shader->set_uniform_int("u_peel_depth", peel_depth);
        pre_phong_shader->set_uniform_float("u_slice_depth", slice_depth);
        pre_phong_shader->set_uniform_vec3f("u_min", min);
        pre_phong_shader->set_uniform_vec3f("u_max", max);
        pre_phong_shader->set_uniform_vec3f("u_slice_direction", slice_direction);
        pre_phong_shader->set_uniform_bool("u_slice_locked", obj->get_data().m_slice_locked);

        vao->draw();

        pre_phong_shader->unbind();
    }
}