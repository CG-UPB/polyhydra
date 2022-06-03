#pragma once

#include "vospch.h"

namespace vOS
{
    class VertexArrayObject
    {
    public:

        /**
         * Renders a full screen quad.
         */
        static void draw_screen_quad();

        /**
         * Creates a new VertexArrayObject from vertices and indices.
         *
         * @param vertices vector of vertices, aligned like so [x0, y0, z0, x1, y1, z1, ...]
         * @param indices vector of indices making up the triangles, for example [0, 1, 2, 2, 3, 0, ...]
         */
        VertexArrayObject(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);

        /**
         * Deletes this VertexArrayObject.
         */
        ~VertexArrayObject();

        /**
         * Updates the vertices and indices with new values.
         *
         * @param vertices new vertices
         * @param indices new indices
         */
        void update_vertices(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);

        /**
         * Draws a single instance of this Vertex Array.
         */
        void draw() const;

        /**
         * Draws multiple instances of this Vertex Array.
         *
         * @param num_instances number of instances to be drawn.
         */
        void draw_instanced(int num_instances) const;

        /**
         * Adds an additional vertex attribute to this vao.
         *
         * @tparam T type of the vertex attribute
         * @param data vertex attribute data, like so [nx0, ny0, nz0, nx1, ny1, nz1, ...]
         * @param location location of vertex attribute (for example, in the shader: layout (location=2) vec3 a_normal)
         * @param element_count number of values for each attribute, for example: float -> 1, vec3 -> 3, mat4 -> 16
         * @param per_instance if true, attribute is added not per vertex, but per instance (use only for instancing)
         */
        template<typename T>
        void add_attribute(const std::vector<T>& data, int location, int element_count, bool per_instance = false);

        /**
         * Updates a buffer at a specific location with new data.
         *
         * @tparam T type of the buffer data
         * @param data new buffer data
         * @param location location of the attribute to be updated
         */
        template<typename T>
        void update_attribute(const std::vector<T>& data, int location);

        /**
         * Use this method to if you only want to update a small portion of the buffer.
         *
         * @tparam T type of the buffer data
         * @param data new buffer data (the whole vector, not some part of it)
         * @param location location of the attribute to be updated
         * @param offset start position of the new data in the buffer
         * @param size number of buffer entries to be updated
         */
        template<typename T>
        void update_attribute(const std::vector<T>& data, int location, int offset, int size);

    private:

        static std::unique_ptr<VertexArrayObject> s_screen_quad;

        // number of vertex indices
        int m_numIndices;

        // opengl buffer pointers
        unsigned int m_vao;
        unsigned int m_vbo;
        unsigned int m_ibo;

        // optional additional buffers
        std::vector<unsigned int> m_buffers;
        std::unordered_map<int, int> m_location_buffer_index;
    };
}