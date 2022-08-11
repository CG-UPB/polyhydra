#pragma once

#include "vospch.h"

namespace volumeshOS::Internal
{
    /**
     * This class represents a Sphere, generated from an Icosahedron.
     */
    class IcoSphereMesh
    {

    public:

        /**
         * Generates a sphere mesh with a given recursion level. A higher level means are higher vertex count.
         *
         * @param recursion_level recursion level
         */
        explicit IcoSphereMesh(int recursion_level);

        /**
         * Returns a list of vertices of this sphere.
         *
         * @return list of vertices
         */
        const std::vector<float>& vertices();

        /**
         * Returns a list of indices for this sphere.
         *
         * @return list of indices
         */
        const std::vector<uint32_t>& indices();

        /**
         * Returns a list of normals for this sphere
         *
         * @return list of normals
         */
        const std::vector<float>& normals();

    private:

        /**
         * Face indices.
         */
        struct Face
        {
            uint32_t v0;
            uint32_t v1;
            uint32_t v2;
        };

        /**
         * Returns the mid vertex of two given vertices.
         *
         * @param v0 first vertex
         * @param v1 second vertex
         * @return
         */
        uint32_t get_mid_point(uint32_t v0, uint32_t v1);

        /**
         * Adds a vertex to this sphere.
         *
         * @param x x value
         * @param y y value
         * @param z z value
         * @return index of the added vertex
         */
        uint32_t add_vertex(float x, float y, float z);

        // sphere data
        uint32_t m_index;
        std::vector<float> m_vertices;
        std::vector<uint32_t> m_indices;
        std::vector<float> m_normals;
        std::unordered_map<long, uint32_t> m_mid_point_cache;
    };
}
