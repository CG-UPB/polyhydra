#pragma once

#include <vector>

namespace vOS
{
    struct CommonShapes
    {
        struct Quad {
            static std::vector<float> vertices(float scale = 1.0) {
                float half_scale = scale * 0.5f;
                return {
                        -half_scale, half_scale, 0.0,
                        -half_scale, -half_scale, 0.0,
                        half_scale, -half_scale, 0.0,
                        half_scale, half_scale, 0.0
                };
            }
            static std::vector<unsigned int> indices() {
                return {
                        0, 1, 2,
                        2, 3, 0
                };
            }
            static std::vector<float> uvs() {
                return {
                        0.0, 0.0,
                        0.0, 1.0,
                        1.0, 1.0,
                        1.0, 0.0
                };
            }
        };
    };
}
