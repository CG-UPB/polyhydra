#pragma once

#include "vospch.h"

namespace vOS
{
    /**
     * Represents a closed cylinder, where both ends are closed.
     */
    class ClosedCylinderMesh
    {

    public:

        /**
         * Creates a new cylinder, where the number of samples determine the number of circle vertices on both ends.
         *
         * @param samples number of vertices for each circle
         * @param radius radius of the cylinder
         * @param height height of the cylinder
         */
        explicit ClosedCylinderMesh(int samples = 10, float radius = 1.0f, float height = 1.0f);
        ~ClosedCylinderMesh() = default;

        /**
         * Returns the vertices of this cylinder.
         *
         * @return list of vertices
         */
        const std::vector<float>& vertices();

        /**
         * Returns the indices of this cylinder.
         *
         * @return list of indices
         */
        const std::vector<unsigned int>& indices();

        /**
         * Returns the normals of this cylinder.
         *
         * @return list of normals
         */
        const std::vector<float>& normals();

    private:

        /**
         * Adds a new vertex to this cylinder
         *
         * @param x x value
         * @param y y value
         * @param z z value
         */
        void add_vertex(float x, float y, float z);

        /**
         * Adds a new normal to this cylinder.
         *
         * @param x x value
         * @param y y value
         * @param z z value
         */
        void add_normal(float x, float y, float z);

        /**
         * Adds a new triangle to this cylinder
         *
         * @param v0 first vertex
         * @param v1 second vertex
         * @param v2 third vertex
         */
        void add_triangle(int v0, int v1, int v2);

        // cylinder data
        std::vector<float> m_vertices;
        std::vector<unsigned int> m_indices;
        std::vector<float> m_normals;
    };
}