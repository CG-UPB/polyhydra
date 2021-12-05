
#include "glad/glad.h"

#include "PixelBufferObject.h"

namespace vOS
{
    PixelBufferObject::PixelBufferObject(unsigned int num_buffers, unsigned int width, unsigned int height):
        m_num_buffers(num_buffers),
        m_width(width),
        m_height(height)
    {
        m_pbo_ids = new unsigned int[num_buffers];
        glGenBuffers((int) num_buffers, m_pbo_ids);
        for (int i = 0; i < num_buffers; i++)
        {
            glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pbo_ids[i]);
            glBufferData(GL_PIXEL_PACK_BUFFER, width * height * 4, nullptr, GL_STREAM_READ);
        }
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    }

    PixelBufferObject::~PixelBufferObject()
    {
        glDeleteBuffers((int) m_num_buffers, m_pbo_ids);
        delete[] m_pbo_ids;
    }

    GLubyte* PixelBufferObject::start_read(int x, int y, int width, int height)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // swap the current buffers
        m_current_index = (m_current_index + 1) % (int) m_num_buffers;
        int next_index = (m_current_index + 1) % (int) m_num_buffers;

        // start reading from the previous pbo
        glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pbo_ids[m_current_index]);
        glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        // now read the actual data from the next pbo
        glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pbo_ids[next_index]);
        return (GLubyte*) glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
    }

    void PixelBufferObject::finish_read()
    {
        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    }
}
