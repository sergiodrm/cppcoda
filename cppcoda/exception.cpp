#include "common.h"

#include <cassert>

namespace coda
{
    ExceptionHandler g_exceptionHandler = nullptr;

    void setExceptionHandler(ExceptionHandler handler)
    {
        g_exceptionHandler = handler;
    }

    void dumpException(const char* message, const char* file, int line)
    {
        printf("Unhandled exception: %s\nFile: %s\nLine: %d\n", message, file, line);
    }

    void throwException(const char* message, const char* file, int line)
    {
        dumpException(message, file, line);
        if (g_exceptionHandler)
        {
            g_exceptionHandler(message, file, line);
            assert(false && "");
        }
        else
        {
            assert(false && "No exception handler set");
        }
    }
}