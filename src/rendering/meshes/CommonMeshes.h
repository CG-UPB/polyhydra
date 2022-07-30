#pragma once

#include "vospch.h"

#include "IcoSphereMesh.h"
#include "ClosedCylinderMesh.h"
#include "CylinderMesh.h"

namespace volumeshOS::Internal
{
    /**
     * Mainly used for VertexArrayObjects, this struct generates common mesh shapes such as planes, cubes and spheres.
     */
    struct CommonMeshes
    {
        /**
         * A plane in xy direction (wall front)
         */
        struct PlaneXY
        {
            /**
             * Generates plane vertices based on size and offset.
             *
             * @param size_x size in x direction
             * @param size_y size in y direction
             * @param offset_z offset in z direction
             * @return vector of vertex coordinates
             */
            inline static std::vector<float> vertices(float size_x = 1.0f, float size_y = 1.0f, float offset_z = 0.0f)
            {
                float half_size_x = size_x * 0.5f;
                float half_size_y = size_y * 0.5f;
                return std::vector<float>{
                        -half_size_x, half_size_y, offset_z,
                        -half_size_x, -half_size_y, offset_z,
                        half_size_x, -half_size_y, offset_z,
                        half_size_x, half_size_y, offset_z
                };
            }

            /**
             * Generates the indices for the plane with an offset.
             *
             * @param offset index offset
             * @return vector of indices
             */
            inline static std::vector<unsigned int> indices(unsigned int offset = 0)
            {
                return std::vector<unsigned int>{
                        offset + 0, offset + 1, offset + 2,
                        offset + 2, offset + 3, offset + 0
                };
            }

            /**
             * Generates the uv texture coordinates for the plane.
             *
             * @return vector of uvs
             */
            inline static std::vector<float> uvs()
            {
                return std::vector<float>{
                        0.0, 0.0,
                        0.0, 1.0,
                        1.0, 1.0,
                        1.0, 0.0
                };
            }

            /**
             * Generates the vertex normals for the plane.
             *
             * @param dir normal direction
             * @return vector of normals
             */
            inline static std::vector<float> normals(float dir = 1.0f)
            {
                return std::vector<float>{
                        0.0, 0.0, dir,
                        0.0, 0.0, dir,
                        0.0, 0.0, dir,
                        0.0, 0.0, dir,
                };
            }
        };

        /**
         * A plane in xz direction (floor)
         */
        struct PlaneXZ
        {
            /**
             * Generates plane vertices based on size and offset.
             *
             * @param size_x size in x direction
             * @param size_z size in z direction
             * @param offset_y offset in y direction
             * @return vector of vertex coordinates
             */
            inline static std::vector<float> vertices(float size_x = 1.0f, float size_z = 1.0f, float offset_y = 0.0f)
            {
                float half_size_x = size_x * 0.5f;
                float half_size_z = size_z * 0.5f;
                return std::vector<float>{
                        -half_size_x, offset_y, half_size_z,
                        -half_size_x, offset_y, -half_size_z,
                        half_size_x, offset_y, -half_size_z,
                        half_size_x, offset_y, half_size_z,
                };
            }

            /**
             * Generates the indices for the plane with an offset.
             *
             * @param offset index offset
             * @return vector of indices
             */
            inline static std::vector<unsigned int> indices(unsigned int offset = 0)
            {
                return PlaneXY::indices(offset);
            }

            /**
             * Generates the uv texture coordinates for the plane.
             *
             * @return vector of uvs
             */
            inline static std::vector<float> uvs()
            {
                return PlaneXY::uvs();
            }

            /**
             * Generates the vertex normals for the plane.
             *
             * @param dir normal direction
             * @return vector of normals
             */
            inline static std::vector<float> normals(float dir = 1.0f)
            {
                return std::vector<float>{
                        0.0, dir, 0.0,
                        0.0, dir, 0.0,
                        0.0, dir, 0.0,
                        0.0, dir, 0.0,
                };
            }
        };

        /**
         * A plane in yz direction (wall side)
         */
        struct PlaneYZ
        {
            /**
             * Generates plane vertices based on size and offset.
             *
             * @param size_y size in y direction
             * @param size_z size in z direction
             * @param offset_x offset in x direction
             * @return vector of vertex coordinates
             */
            inline static std::vector<float> vertices(float size_y = 1.0f, float size_z = 1.0f, float offset_x = 0.0f)
            {
                float half_size_y = size_y * 0.5f;
                float half_size_z = size_z * 0.5f;
                return std::vector<float>{
                        offset_x, -half_size_y, half_size_z,
                        offset_x, -half_size_y, -half_size_z,
                        offset_x, half_size_y, -half_size_z,
                        offset_x, half_size_y, half_size_z,
                };
            }

            /**
             * Generates the indices for the plane with an offset.
             *
             * @param offset index offset
             * @return vector of indices
             */
            inline static std::vector<unsigned int> indices(unsigned int offset = 0)
            {
                return PlaneXY::indices(offset);
            }

            /**
             * Generates the uv texture coordinates for the plane.
             *
             * @return vector of uvs
             */
            inline static std::vector<float> uvs()
            {
                return PlaneXY::uvs();
            }

            /**
             * Generates the vertex normals for the plane.
             *
             * @param dir normal direction
             * @return vector of normals
             */
            inline static std::vector<float> normals(float dir = -1.0f)
            {
                return std::vector<float>{
                        dir, 0.0, 0.0,
                        dir, 0.0, 0.0,
                        dir, 0.0, 0.0,
                        dir, 0.0, 0.0,
                };
            }
        };

        /**
         * A box mesh.
         */
        struct Box
        {
            /**
             * Generates a list of vertices for the box.
             *
             * @param size_x size in x direction
             * @param size_y size in y direction
             * @param size_z size in z direction
             * @return vector of vertex coordinates
             */
            inline static std::vector<float> vertices(float size_x = 1.0f, float size_y = 1.0f, float size_z = 1.0f)
            {
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

            /**
             * Generates the indices for the box.
             *
             * @return vector of indices
             */
            inline static std::vector<unsigned int> indices()
            {
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

            /**
             * Generates the normals for the box.
             *
             * @return vector of normals
             */
            inline static std::vector<float> normals()
            {
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

        /**
         * A sphere mesh.
         */
        struct Sphere
        {

        private:

            /**
             * This mesh won't change, so only generate it once to save performance.
             *
             * @return a sphere with the default recursion level
             */
            static IcoSphereMesh& default_sphere()
            {
                static IcoSphereMesh s_default_sphere = IcoSphereMesh(2);
                return s_default_sphere;
            }

        public:

            /**
             * This is the sphere that gets drawn for each vertex during the selection pass, it has the lowest
             * recursion level to save performance.
             *
             * @return a low resolution sphere
             */
            static IcoSphereMesh& selection_sphere()
            {
                static IcoSphereMesh s_selection_sphere = IcoSphereMesh(0);
                return s_selection_sphere;
            }

            /**
             * Generates a list of vertices for the sphere.
             *
             * @return vector of vertex coordinates
             */
            static const std::vector<float>& vertices()
            {
                return default_sphere().vertices();
            }

            /**
             * Generates a list of indices for the sphere.
             *
             * @return vector of indices
             */
            static const std::vector<unsigned int>& indices()
            {
                return default_sphere().indices();
            }

            /**
             * Generates a list of normals for the sphere.
             *
             * @return list of normals
             */
            static const std::vector<float>& normals()
            {
                return default_sphere().normals();
            }
        };

        struct Cylinder
        {

        private:

            /**
             * This mesh won't change, so only generate it once to save performance.
             *
             * @return a cylinder with average resolution
             */
            static ClosedCylinderMesh& default_cylinder()
            {
                static ClosedCylinderMesh s_default_cylinder = ClosedCylinderMesh(20, 0.5f, 1.0f);
                return s_default_cylinder;
            }

        public:

            /**
             * This cylinder gets drawn for each edge in the selection pass, so it has a relatively low resolution
             * to save performance.
             *
             * @return a low resolution cylinder
             */
            static CylinderMesh& edge_cylinder()
            {
                static CylinderMesh s_edge_cylinder = CylinderMesh(5, 0.5f, 1.0f);
                return s_edge_cylinder;
            }

            /**
             * This cylinder gets drawn for each edge hover.
             *
             * @return a medium resolution cylinder
             */
            static CylinderMesh& edge_hover_cylinder()
            {
                static CylinderMesh s_edge_hover = CylinderMesh(20, 0.5f, 1.0f);
                return s_edge_hover;
            }

            /**
             * Generates a list of vertices for this cylinder.
             *
             * @return list of vertices
             */
            static const std::vector<float>& vertices()
            {
                return default_cylinder().vertices();
            }

            /**
             * Generates a list of indices for this cylinder.
             *
             * @return list of indices
             */
            static const std::vector<unsigned int>& indices()
            {
                return default_cylinder().indices();
            }

            /**
             * Generates a list of normals for this cylinder.
             *
             * @return list of normals
             */
            static const std::vector<float>& normals()
            {
                return default_cylinder().normals();
            }
        };
    };
}
