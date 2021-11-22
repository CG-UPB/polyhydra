#pragma once

#include <vector>

namespace vOS
{
    struct CommonMeshes
    {
        struct PlaneXY {
            inline static std::vector<float> vertices(float size_x = 1.0f, float size_y = 1.0f, float offset_z = 0.0f) {
                float half_size_x = size_x * 0.5f;
                float half_size_y = size_y * 0.5f;
                return std::vector<float> {
                        -half_size_x, half_size_y, offset_z,
                        -half_size_x, -half_size_y, offset_z,
                        half_size_x, -half_size_y, offset_z,
                        half_size_x, half_size_y, offset_z
                };
            }
            inline static std::vector<unsigned int> indices(unsigned int offset = 0) {
                return std::vector<unsigned int> {
                        offset + 0, offset + 1, offset + 2,
                        offset + 2, offset + 3, offset + 0
                };
            }
            inline static std::vector<float> uvs() {
                return std::vector<float> {
                        0.0, 0.0,
                        0.0, 1.0,
                        1.0, 1.0,
                        1.0, 0.0
                };
            }
            inline static std::vector<float> normals(float dir = 1.0f) {
                return std::vector<float> {
                        0.0, 0.0, dir,
                        0.0, 0.0, dir,
                        0.0, 0.0, dir,
                        0.0, 0.0, dir,
                };
            }
        };

        struct PlaneXZ {
            inline static std::vector<float> vertices(float size_x = 1.0f, float size_z = 1.0f, float offset_y = 0.0f) {
                float half_size_x = size_x * 0.5f;
                float half_size_z = size_z * 0.5f;
                return std::vector<float> {
                        -half_size_x, offset_y, half_size_z,
                        -half_size_x, offset_y, -half_size_z,
                        half_size_x, offset_y, -half_size_z,
                        half_size_x, offset_y, half_size_z,
                };
            }
            inline static std::vector<unsigned int> indices(unsigned int offset = 0) {
                return PlaneXY::indices(offset);
            }
            inline static std::vector<float> uvs() {
                return PlaneXY::uvs();
            }
            inline static std::vector<float> normals(float dir = 1.0f) {
                return std::vector<float> {
                        0.0, dir, 0.0,
                        0.0, dir, 0.0,
                        0.0, dir, 0.0,
                        0.0, dir, 0.0,
                };
            }
        };

        struct PlaneYZ {
            inline static std::vector<float> vertices(float size_y = 1.0f, float size_z = 1.0f, float offset_x = 0.0f) {
                float half_size_y = size_y * 0.5f;
                float half_size_z = size_z * 0.5f;
                return std::vector<float> {
                        offset_x, -half_size_y, half_size_z,
                        offset_x, -half_size_y, -half_size_z,
                        offset_x, half_size_y, -half_size_z,
                        offset_x, half_size_y, half_size_z,
                };
            }
            inline static std::vector<unsigned int> indices(unsigned int offset = 0) {
                return PlaneXY::indices(offset);
            }
            inline static std::vector<float> uvs() {
                return PlaneXY::uvs();
            }
            inline static std::vector<float> normals(float dir = -1.0f) {
                return std::vector<float> {
                        dir, 0.0, 0.0,
                        dir, 0.0, 0.0,
                        dir, 0.0, 0.0,
                        dir, 0.0, 0.0,
                };
            }
        };

        struct Box {
            inline static std::vector<float> vertices(float size_x = 1.0f, float size_y = 1.0f, float size_z = 1.0f) {
                std::vector<float> res;
                float half_size_x = size_x * 0.5f;
                float half_size_y = size_y * 0.5f;
                float half_size_z = size_z * 0.5f;
                std::vector<float> top = PlaneXZ::vertices(size_x, size_z, half_size_y);
                std::vector<float> bottom = PlaneXZ::vertices(size_x, size_z, -half_size_y);
                std::vector<float> left = PlaneYZ::vertices(size_y, size_z, -half_size_x);
                std::vector<float> right = PlaneYZ::vertices(size_y, size_z, half_size_x);
                std::vector<float> back = PlaneXY::vertices(size_x, size_y, -half_size_z);
                std::vector<float> front = PlaneXY::vertices(size_x, size_y, half_size_z);
                res.insert(res.end(), top.begin(), top.end());
                res.insert(res.end(), bottom.begin(), bottom.end());
                res.insert(res.end(), left.begin(), left.end());
                res.insert(res.end(), right.begin(), right.end());
                res.insert(res.end(), back.begin(), back.end());
                res.insert(res.end(), front.begin(), front.end());
                return res;
            }
            inline static std::vector<unsigned int> indices() {
                std::vector<unsigned int> res;
                std::vector<unsigned int> top = PlaneXZ::indices(0);
                std::vector<unsigned int> bottom = PlaneXZ::indices(4);
                std::vector<unsigned int> left = PlaneYZ::indices(8);
                std::vector<unsigned int> right = PlaneYZ::indices(12);
                std::vector<unsigned int> back = PlaneXY::indices(16);
                std::vector<unsigned int> front = PlaneXY::indices(20);
                res.insert(res.end(), top.begin(), top.end());
                res.insert(res.end(), bottom.begin(), bottom.end());
                res.insert(res.end(), left.begin(), left.end());
                res.insert(res.end(), right.begin(), right.end());
                res.insert(res.end(), back.begin(), back.end());
                res.insert(res.end(), front.begin(), front.end());
                return res;
            }
            inline static std::vector<float> normals() {
                std::vector<float> res;
                std::vector<float> top = PlaneXZ::normals(1.0f);
                std::vector<float> bottom = PlaneXZ::normals(-1.0f);
                std::vector<float> left = PlaneYZ::normals(-1.0f);
                std::vector<float> right = PlaneYZ::normals(1.0f);
                std::vector<float> back = PlaneXY::normals(-1.0f);
                std::vector<float> front = PlaneXY::normals(1.0f);
                res.insert(res.end(), top.begin(), top.end());
                res.insert(res.end(), bottom.begin(), bottom.end());
                res.insert(res.end(), left.begin(), left.end());
                res.insert(res.end(), right.begin(), right.end());
                res.insert(res.end(), back.begin(), back.end());
                res.insert(res.end(), front.begin(), front.end());
                return res;
            }
        };
    };
}
