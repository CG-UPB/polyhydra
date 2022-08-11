#pragma once

#include "vospch.h"

namespace volumeshOS::Internal
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
        PixelBufferObject(uint32_t num_buffers, uint32_t width, uint32_t height);
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
        uint32_t m_width;
        uint32_t m_height;
        uint32_t m_num_buffers;
        std::unique_ptr<uint32_t> m_pbo_ids;
    };
}