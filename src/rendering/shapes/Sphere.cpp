
#include "Sphere.h"

#include "CommonMeshes.h"

namespace vOS
{
    Sphere::~Sphere()
    {
        delete m_vao;
    }

    void Sphere::draw()
    {
        if (m_vao == nullptr)
        {
            m_vao = new VertexArrayObject(CommonMeshes::Sphere::vertices(), CommonMeshes::Sphere::indices());
            m_vao->add_attribute(CommonMeshes::Sphere::normals(), 1, 3);
        }
        m_vao->draw();
    }
}