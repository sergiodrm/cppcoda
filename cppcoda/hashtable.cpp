#include "hashtable.h"

namespace coda
{
    uint64 hash_function(const char* key)
    {
        uint64 h = 0;
        uint64 i = 0;
        while (key[i]) h+=key[i++];
        return h;
    }
}