#pragma once

#include "vospch.h"

namespace vOS
{
    class GLUtil
    {
    public:

        template<typename... Args>
        static inline void pushConfig(int field, Args... args)
        {

            pushConfig(args...);
        }
    };
}