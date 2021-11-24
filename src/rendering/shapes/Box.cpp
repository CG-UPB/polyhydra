#include "Box.h"

#include "CommonMeshes.h"

namespace vOS
{

    Box::Box(float size_x, float size_y, float size_z): m_size(size_x, size_y, size_z)
    {}

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