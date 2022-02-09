#pragma once

#include <cstdint>

namespace vOS
{
    class PixelBufferObject
    {
    public:

        explicit PixelBufferObject(unsigned int num_buffers, unsigned int width, unsigned int height);
        ~PixelBufferObject();

        uint8_t* start_read(int x, int y, int width, int height);
        void finish_read();

    private:

        int m_current_index = 0;

        unsigned int m_width;
        unsigned int m_height;
        unsigned int m_num_buffers;
        unsigned int* m_pbo_ids;
    };
}