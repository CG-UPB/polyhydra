#pragma once

namespace volumeshOS::Internal
{
    struct ConeMesh
    {
        explicit ConeMesh(int samples = 20, float radius = 0.5f, float height = 1.0f);

        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        std::vector<float> normals;

    private:

        void add_vertex(float x, float y, float z);

        void add_triangle(int v0, int v1, int v2);

        void add_normal(float x, float y, float z);
    };
}