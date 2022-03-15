
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
        glDisable(GL_BLEND);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        auto pre_phong_shader = Shader::pre_mesh_phong_shader();

        pre_phong_shader->bind();

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
        glm::vec3 light_pos(data.camera.view * glm::vec4(data.light.position, 1.0));

        // set all of our uniforms
        pre_phong_shader->set_uniform_mat4f("u_transform", transform);
        pre_phong_shader->set_uniform_mat4f("u_projection", data.camera.projection);
        pre_phong_shader->set_uniform_mat4f("u_view", data.camera.view);
        pre_phong_shader->set_uniform_vec3f("u_light_pos", light_pos);
        pre_phong_shader->set_uniform_vec3f("u_cam_pos", cam_pos);
        pre_phong_shader->set_uniform_vec3f("u_light_color", data.light.color);
        pre_phong_shader->set_uniform_float("u_cell_size", cell_size);
        pre_phong_shader->set_uniform_vec3f("u_object_color", obj->get_data().m_color.get_rgba());
        pre_phong_shader->set_uniform_float("u_peel_depth", peel_depth);
        pre_phong_shader->set_uniform_float("u_slice_depth", slice_depth);
        pre_phong_shader->set_uniform_vec3f("u_min", min);
        pre_phong_shader->set_uniform_vec3f("u_max", max);
        pre_phong_shader->set_uniform_vec3f("u_slice_direction", slice_direction);
        pre_phong_shader->set_uniform_bool("u_slice_locked", obj->get_data().m_slice_locked);
        pre_phong_shader->set_uniform_bool("u_rounding", obj->get_data().m_rounding_activated);
        pre_phong_shader->set_uniform_float("u_rounding_size", obj->get_data().m_rounding_size);
        pre_phong_shader->set_uniform_float("u_average_cell_size", obj->get_mvb()->get_average_cell_size());

        vao->draw();
        
        pre_phong_shader->unbind();
    }

    PrePass::PrePass(int width, int height)
    {
        m_clear_position_shader = Shader::get("pre_far");
        m_pre_pass_framebuffer = new PrePassFrameBufferObject(width, height);
    }

    PrePass::~PrePass()
    {
        delete m_pre_pass_framebuffer;
    }

    void PrePass::resize_buffers(int width, int height)
    {
        m_pre_pass_framebuffer->resize(width, height);
    }

    PrePassFrameBufferObject* PrePass::get_framebuffer() const
    {
        return m_pre_pass_framebuffer;
    }

    void PrePass::clear_position_buffer(const RenderData& data)
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        m_clear_position_shader->bind();
        m_clear_position_shader->set_uniform_float("u_far", data.camera.far);
        VertexArrayObject::draw_screen_quad();
        m_clear_position_shader->unbind();
    }
}