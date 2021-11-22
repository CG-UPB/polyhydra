#include "Box.h"

#include "CommonMeshes.h"

namespace vOS
{

    Box::Box(float size_x, float size_y, float size_z): m_size(size_x, size_y, size_z)
    {
        m_shader = Shader::basic_shape_shader();
        set_transform(glm::translate(glm::vec3(1.0, 0.0, 0.0)) * glm::scale(glm::vec3(0.2, 0.2, 0.2)));
        set_base_color(glm::vec4(1.0, 0.2, 0.2, 1.0));
    }

    Box::~Box()
    {
        delete m_vao;
    }

    void Box::draw()
    {
        if (m_vao == nullptr)
        {
            m_vao = new VertexArrayObject(CommonMeshes::Box::vertices(m_size.x, m_size.y, m_size.z), CommonMeshes::Box::indices());
            m_vao->add_buffer(CommonMeshes::Box::normals(), 1, 3);
        }
        m_vao->draw();
    }
}