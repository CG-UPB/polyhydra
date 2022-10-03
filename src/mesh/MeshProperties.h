#pragma once

#include "vospch.h"

namespace volumeshOS::Internal
{
    class MeshProperties
    {
    public:

        static constexpr const char* PROP_PEEL_DEPTH = "PeelDepth";

        static constexpr const char* PROP_IS_BEZIER = "IsBezierMesh";
        static constexpr const char* PROP_BEZIER_DEGREE = "BezierDegree";
        static constexpr const char* PROP_BEZIER_FACE_CONTROL_POINTS = "BezierFaceControlPoints";

        static inline void setup_mesh_properties(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>& mesh)
        {
            add_property<int, OpenVolumeMesh::Entity::Cell>(mesh, PROP_PEEL_DEPTH, 0);
            add_property<int, OpenVolumeMesh::Entity::Vertex>(mesh, PROP_PEEL_DEPTH, 0);
        
            // Add Bezier Mesh properties if they do not exist
            add_property<bool, OpenVolumeMesh::Entity::Mesh>(mesh, PROP_IS_BEZIER, false);
            add_property<int, OpenVolumeMesh::Entity::Mesh>(mesh, PROP_BEZIER_DEGREE, 0);
            add_property<std::vector<double>, OpenVolumeMesh::Entity::Face>(mesh, PROP_BEZIER_FACE_CONTROL_POINTS, std::vector<double>());
        }

    private:

        template<typename T, typename E>
        static inline void add_property(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>& mesh, const char* name, const T& default_value)
        {
            mesh.request_property<T, E>(name, default_value)->set_persistent(true);
        }
    };
}
