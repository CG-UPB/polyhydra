#include "RingBuffer.h"

RingBuffer::RingBuffer() = default;
RingBuffer::RingBuffer(size_t size): max_size(size){}

void RingBuffer::push(int value)
{
    if (buffer.size() == max_size)
    {
        buffer.pop_front();
        buffer.push_back(value);
    }
    buffer.push_back(value);
}

void RingBuffer::pop()
{
    if (!buffer.empty())
    {
        buffer.pop_back();
    }
}

void RingBuffer::clear()
{
    buffer.clear();
}

bool RingBuffer::is_empty()
{
    return buffer.empty();
}

int RingBuffer::get_latest()
{
    return buffer.back();
}
