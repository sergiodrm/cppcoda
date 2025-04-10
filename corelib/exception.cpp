#include "common.h"

#include <cassert>

namespace clb
{
    ExceptionHandler g_exceptionHandler = nullptr;

    void setExceptionHandler(ExceptionHandler handler)
    {
        g_exceptionHandler = handler;
    }

    void throwException(const char* message, const char* file, int line)
    {
        if (g_exceptionHandler)
        {
            g_exceptionHandler(message, file, line);
        }
        else
        {
            assert(false && "No exception handler set");
        }
    }
}