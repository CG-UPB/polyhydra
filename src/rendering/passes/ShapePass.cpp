
#include "glad/glad.h"

#include "ShapePass.h"

namespace vOS
{
    std::vector<Shape*> ShapePass::s_shapes;

    ShapePass::~ShapePass()
    {
        for (const Shape* shape : s_shapes)
        {
            delete shape;
        }
    }

    void ShapePass::render(const VertexArrayObject& vao, const RenderData& data)
    {
        glm::mat4 positionOffset = glm::translate(-data.mesh.offset);
        glm::mat4 transform = data.camera.world * data.mesh.transform * positionOffset;

        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);

        for (Shape* shape : s_shapes)
        {

            Shader& shader = *shape->get_shader();
            shader.bind();

            shader.set_uniform_mat4f("u_mesh_transform", transform);
            shader.set_uniform_mat4f("u_projection", data.camera.projection);
            shader.set_uniform_mat4f("u_view", data.camera.view);

            shader.set_uniform_vec3f("u_light_pos", data.light.position);
            shader.set_uniform_vec3f("u_cam_pos", data.camera.position);
            shader.set_uniform_vec3f("u_light_color", data.light.color);
            shader.set_uniform_bool("u_phong", true);

            shader.set_uniform_mat4f("u_transform", shape->get_transform());
            shader.set_uniform_vec4f("u_base_color", shape->get_base_color());

            shape->draw();

            shader.unbind();
        }
    }

    void ShapePass::add_shape(Shape* shape)
    {
        s_shapes.push_back(shape);
    }

    const Shape& ShapePass::get_shape(unsigned int shape_id)
    {
        if (shape_id < 0 || shape_id >= s_shapes.size())
        {
            throw std::invalid_argument("Shape with id " + std::to_string(shape_id) + " does not exist");
        }
        return *s_shapes[shape_id];
    }
}