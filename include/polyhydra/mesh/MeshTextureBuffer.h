#pragma once

#include "polyhydra/polyhydraPCH.h"

namespace polyhydra::Internal
{

typedef OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d> Mesh;

class MeshTextureBuffer
{
  public:
    MeshTextureBuffer(std::shared_ptr<Mesh> mesh);
    MeshTextureBuffer(std::shared_ptr<Mesh> mesh, uint32_t binding);
    virtual ~MeshTextureBuffer();

    void bind() const;
    uint32_t get_binding() const;
    uint32_t get_texture() const;
    void update_buffer(uint32_t size, const std::vector<float>& data) const;

  private:
    std::shared_ptr<Mesh> m_mesh;
    uint32_t m_bezier_control_points_texture_buffer;
    uint32_t m_bezier_control_points_texture;

    uint32_t m_binding;
    uint32_t m_texture_buffer;
    uint32_t m_texture;
};

} // namespace polyhydra::Internal