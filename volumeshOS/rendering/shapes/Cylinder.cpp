
#include "Cylinder.h"
#include "../meshes/CommonMeshes.h"

namespace vOS
{

    Cylinder::~Cylinder()
    {
        delete m_vao;
    }

    void Cylinder::draw()
    {
        if (m_vao == nullptr)
        {
            m_vao = new VertexArrayObject(CommonMeshes::Cylinder::vertices(), CommonMeshes::Cylinder::indices());
            m_vao->add_attribute(CommonMeshes::Cylinder::normals(), 1, 3);
        }
        m_vao->draw();
    }
}