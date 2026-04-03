#pragma once

namespace polyhydra::Internal
{
struct ArrowMesh
{
    explicit ArrowMesh(int samples = 20, float base_width_percentage = 0.5f, float tip_height_percentage = 0.3f);

    std::vector<float> vertices = {};
    std::vector<uint32_t> indices = {};
    std::vector<float> normals = {};
};
} // namespace polyhydra::Internal