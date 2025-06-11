#pragma once

#define CODA_USE_STD

#define coda_dummy_macro ((void)0)

#define coda_assert(x) ((x) ? coda_dummy_macro : coda::throwException("Assertion failed: "#x, __FILE__, __LINE__))
#define coda_assert_msg(x, msg) ((x) ? coda_dummy_macro : coda::throwException("Assertion failed: " msg, __FILE__, __LINE__))

#ifdef _DEBUG
#define coda_dbg_assert(x) coda_assert(x)
#define coda_dbg_assert_msg(x, msg) coda_assert_msg(x,msg)
#else
#define coda_dbg_assert(x) coda_dummy_macro
#define coda_dbg_assert_msg(x, msg) coda_dummy_macro
#endif


#ifdef CODA_USE_STD
#include <limits>
#else
#error Currently not supported without some stdlib functionalities
#endif


namespace coda
{
    // Commonly used types
    using byte = unsigned char;
    using int8 = signed char;
    using int16 = signed short;
    using int32 = signed int;
    using int64 = signed long long;
    using uint8 = unsigned char;
    using uint16 = unsigned short;
    using uint32 = unsigned int;
    using uint64 = unsigned long long;

    // type limits
    template <typename T>
    struct TypeLimit
    {
        static constexpr T min() { return std::numeric_limits<T>::min(); }
        static constexpr T max() { return std::numeric_limits<T>::max(); }
    };

    // Global index type
    using index_t = size_t;

    // Exception handling
    typedef void (*ExceptionHandler)(const char* message, const char* file, int line);
    void setExceptionHandler(ExceptionHandler handler);
    void dumpException(const char* message, const char* file, int line);
    void throwException(const char* message, const char* file, int line);

    // Cast functions with limit checking
    template <typename T, typename U>
    T safe_cast(U value)
    {
        coda_assert(value >= static_cast<U>(TypeLimit<T>::min()) && value <= static_cast<U>(TypeLimit<T>::max()));
        return static_cast<T>(value);
    }
}