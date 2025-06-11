#include "allocator.h"
#include <cstdlib>

namespace coda
{
    void* baseallocator::allocate(size_t size)
    {
        return malloc(size);
    }

    void* baseallocator::reallocate(void* p, size_t size)
    {
        return realloc(p, size);
    }

    void baseallocator::release(void* p)
    {
        free(p);
    }
}
