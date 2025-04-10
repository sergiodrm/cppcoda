#pragma once

#define CLB_USE_STD

#define clb_dummy_macro ((void)0)

#define clb_assert(x) ((x) ? clb_dummy_macro : clb::throwException("Assertion failed: "#x, __FILE__, __LINE__))
#define clb_assert_msg(x, msg) ((x) ? clb_dummy_macro : clb::throwException("Assertion failed: " msg, __FILE__, __LINE__))

#ifdef _DEBUG
#define clb_dbg_assert(x) clb_assert(x)
#define clb_dbg_assert_msg(x, msg) clb_assert_msg(x,msg)
#else
#define clb_dbg_assert(x) clb_dummy_macro
#define clb_dbg_assert_msg(x, msg) clb_dummy_macro
#endif


#ifdef CLB_USE_STD
#include <limits>
#else
#error Currently not supported without some stdlib functionalities
#endif


namespace clb
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
    void throwException(const char* message, const char* file, int line);

    // Cast functions with limit checking
    template <typename T, typename U>
    T safe_cast(U value)
    {
        clb_assert(value >= static_cast<U>(TypeLimit<T>::min()) && value <= static_cast<U>(TypeLimit<T>::max()));
        return static_cast<T>(value);
    }
}