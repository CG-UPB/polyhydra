//
// Created by steffen on 11.10.21.
//

#ifndef VOLUMESHOS_VERTEXARRAYOBJECT_H
#define VOLUMESHOS_VERTEXARRAYOBJECT_H

#include <vector>

namespace vOS
{
    class VertexArrayObject
    {
    public:
        VertexArrayObject(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
        ~VertexArrayObject();
        void draw();

    private:

        unsigned int m_vao;
        unsigned int m_vbo;
        unsigned int m_ibo;

        int m_numIndices;
    };
}


#endif //VOLUMESHOS_VERTEXARRAYOBJECT_H
