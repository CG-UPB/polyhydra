#pragma once

#include <cstdint>

namespace vOS
{

    /**
     * Represents an OpenGL pixel buffer for accelerated texture read and write operations on the CPU.
     */
    class PixelBufferObject
    {
    public:

        /**
         * Creates a new pixel buffer with a specified width and height, and the number of backend buffers (common: 2)
         *
         * @param num_buffers backend buffers
         * @param width width
         * @param height height
         */
        PixelBufferObject(unsigned int num_buffers, unsigned int width, unsigned int height);
        ~PixelBufferObject();

        /**
         * Reads the pixels from the previous iteration.
         *
         * @param x read x start
         * @param y read y start
         * @param width read width
         * @param height read height
         * @return color values as bytes
         */
        uint8_t* start_read(int x, int y, int width, int height);

        /**
         * Finished the reading operation and prepares buffers for the next read.
         */
        void finish_read();

    private:

        // current backend buffer index to be read from
        int m_current_index = 0;

        // width, height and ids
        unsigned int m_width;
        unsigned int m_height;
        unsigned int m_num_buffers;
        unsigned int* m_pbo_ids;
    };
}