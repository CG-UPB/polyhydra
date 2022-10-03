#pragma once

#include "vospch.h"

namespace volumeshOS::Internal
{

    typedef OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d> Mesh;

    class MeshTextureBuffer
    {
    public:
        MeshTextureBuffer(const std::shared_ptr<Mesh>& mesh);
        virtual ~MeshTextureBuffer();

        void update_buffer();
        void bind() const;
    private:
        std::shared_ptr<Mesh> m_mesh;
        unsigned int m_bezier_control_points_texture_buffer;
        unsigned int m_bezier_control_points_texture;

    };

}