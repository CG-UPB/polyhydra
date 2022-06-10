
#include "ShapePass.h"
#include "../../Window.h"

namespace vOS
{
    // Static Variables
    std::vector<Shape*> ShapePass::s_shapes;

    ShapePass::~ShapePass()
    {
        // Delete all Shapes
        for (const Shape* shape : s_shapes)
        {
            delete shape;
        }
    }

    void ShapePass::render(std::shared_ptr<VertexArrayObject> vao, const RenderData& data, std::shared_ptr<MeshObject> mesh)
    {
        // Translate
        glm::mat4 transform = data.camera.world * mesh->get_data().get_transform();

        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);

        glm::vec3 light_pos(data.camera.view * glm::vec4(data.light.position, 1.0));

        // Renders all Shapes
        for (Shape* shape : s_shapes)
        {
            // Get Shape Shader
            Shader& shader = *shape->get_shader();
            shader.bind();

            // Set uniform variables
            shader.set_uniform_mat4f("u_mesh_transform", transform);
            shader.set_uniform_mat4f("u_projection", data.camera.projection);
            shader.set_uniform_mat4f("u_view", data.camera.view);

            shader.set_uniform_vec3f("u_light_pos", light_pos);
            shader.set_uniform_vec3f("u_cam_pos", data.camera.position);
            shader.set_uniform_vec3f("u_light_color", data.light.color);
            shader.set_uniform_bool("u_phong", true);

            shader.set_uniform_mat4f("u_transform", shape->get_transform());
            shader.set_uniform_vec4f("u_base_color", shape->get_base_color());

            // Draw Shape
            shape->draw();

            shader.unbind();
        }
    }

    void ShapePass::add_shape(Shape* shape)
    {
        s_shapes.push_back(shape);
    }

    void ShapePass::remove_all(){
        // Find and remove all shapes
        for(int i = 0; i< s_shapes.size(); i++){
            remove_shape(s_shapes[i]->get_id());
            i--;
        }
    }

    void ShapePass::remove_shape(unsigned int id){
        // Find Shape
        int entry = 0;
        for(int i = 0; i < s_shapes.size(); i++){
            if(s_shapes[i]->get_id() == id)
                break;
            entry++;
        }
        // Delete Shape if it exists
        if(entry != s_shapes.size()){
            // Found the shape
            auto shape = s_shapes[entry];
            s_shapes.erase(s_shapes.begin()+entry);
            delete shape;
        }
    }

    const Shape& ShapePass::get_shape(unsigned int shape_id)
    {
        // Find Shape
        if (shape_id < 0 || shape_id >= s_shapes.size())
        {
            throw std::invalid_argument("Shape with id " + std::to_string(shape_id) + " does not exist");
        }
        return *s_shapes[shape_id];
    }
}