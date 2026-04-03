#pragma once

#include <deque>

class RingBuffer
{
  public:
    RingBuffer();
    RingBuffer(size_t size);
    void push(int value);
    void pop();
    void clear();
    bool is_empty();
    int get_latest();

  private:
    std::deque<int> buffer;
    size_t max_size;
};
