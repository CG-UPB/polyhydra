
#include "Shapes.h"

#include "../gl/VertexArrayObject.h"
#include "../meshes/CommonMeshes.h"
#include "../meshes/ConeMesh.h"
#include "../meshes/ArrowMesh.h"

namespace volumeshOS::Internal
{
    ShapeType BaseShape::s_current_type_id = 0;

    ShapeType BaseShape::next_type()
    {
        return s_current_type_id++;
    }

    ShapeType BaseShape::get_type() const
    {
        return -1;
    }

    std::unique_ptr<VertexArrayObject> BaseShape::get_vao() const
    {
        return nullptr;
    }

    std::unique_ptr<VertexArrayObject> BoxShape::get_vao() const
    {
        auto vao = std::make_unique<VertexArrayObject>(
                CommonMeshes::Box::vertices(),
                CommonMeshes::Box::indices()
        );
        vao->add_attribute(CommonMeshes::Box::normals(), 1, 3, false);
        return vao;
    }

    std::unique_ptr<VertexArrayObject> CylinderShape::get_vao() const
    {
        auto vao = std::make_unique<VertexArrayObject>(
                CommonMeshes::Cylinder::vertices(),
                CommonMeshes::Cylinder::indices()
        );
        vao->add_attribute(CommonMeshes::Cylinder::normals(), 1, 3, false);
        return vao;
    }

    std::unique_ptr<VertexArrayObject> SphereShape::get_vao() const
    {
        auto vao = std::make_unique<VertexArrayObject>(
                CommonMeshes::Sphere::vertices(),
                CommonMeshes::Sphere::indices()
        );
        vao->add_attribute(CommonMeshes::Sphere::normals(), 1, 3, false);
        return vao;
    }

    std::unique_ptr<VertexArrayObject> ConeShape::get_vao() const
    {
        ConeMesh cone;
        auto vao = std::make_unique<VertexArrayObject>(
                cone.vertices,
                cone.indices
        );
        vao->add_attribute(cone.normals, 1, 3, false);
        return vao;
    }

    std::unique_ptr<VertexArrayObject> ArrowShape::get_vao() const
    {
        ArrowMesh arrow{20, base_width_percentage, tip_height_percentage};
        auto vao = std::make_unique<VertexArrayObject>(
                arrow.vertices,
                arrow.indices
        );
        vao->add_attribute(arrow.normals, 1, 3, false);
        return vao;
    }
}