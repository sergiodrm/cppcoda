#pragma once

#include "common.h"

namespace coda
{
    class baseallocator
    {
    public:
        static void* allocate(size_t size);
        static void* reallocate(void* p, size_t size);
        static void release(void* p);
    };
}