#pragma once

#include "vospch.h"

namespace volumeshOS::Internal
{
    class MeshProperties
    {
    public:

        static constexpr const char* PROP_PEEL_DEPTH = "PeelDepth";

        static inline void setup_mesh_properties(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>& mesh)
        {
            add_property<int, OpenVolumeMesh::Entity::Cell>(mesh, PROP_PEEL_DEPTH, 0);
            add_property<int, OpenVolumeMesh::Entity::Vertex>(mesh, PROP_PEEL_DEPTH, 0);
        }

    private:

        template<typename T, typename E>
        static inline void add_property(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>& mesh, const char* name, const T& default_value)
        {
            mesh.request_property<T, E>(name, default_value)->set_persistent(true);
        }
    };
}
