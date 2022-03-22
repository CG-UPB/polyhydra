#pragma once

#include "vospch.h"

namespace vOS
{
    class MeshProperties
    {
    public:

        static constexpr const char* PROP_PEEL_DEPTH                = "PeelDepth";
        static constexpr const char* PROP_DIGGING                   = "DiggingProperty";
        static constexpr const char* PROP_ISOLATE                   = "IsolateProperty";
        static constexpr const char* PROP_BUFFER_INDEX_SIZE         = "BufferIndexSize";
        static constexpr const char* PROP_BUFFER_INDEX_SIZE_ROUNDED = "BufferIndexSizeRounded";

        static inline void setup_mesh_properties(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>& mesh)
        {
            add_property<int, OpenVolumeMesh::Entity::Cell>(mesh, PROP_PEEL_DEPTH, 0);
            add_property<int, OpenVolumeMesh::Entity::Vertex>(mesh, PROP_PEEL_DEPTH, 0);
            add_property<bool, OpenVolumeMesh::Entity::Cell>(mesh, PROP_DIGGING, true);
            add_property<bool, OpenVolumeMesh::Entity::Cell>(mesh, PROP_ISOLATE, false);
            add_property<glm::ivec2, OpenVolumeMesh::Entity::HalfFace>(mesh, PROP_BUFFER_INDEX_SIZE, {0.0, 0.0});
            add_property<glm::ivec2, OpenVolumeMesh::Entity::HalfFace>(mesh, PROP_BUFFER_INDEX_SIZE_ROUNDED, {0.0, 0.0});
        }

    private:

        template<typename T, typename E>
        static inline void add_property(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>& mesh, const char* name, const T& default_value)
        {
            mesh.request_property<T, E>(name, default_value)->set_persistent(true);
        }
    };
}
