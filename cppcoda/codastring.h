#pragma once

#include "common.h"
#include "allocator.h"
#include <cstdarg>
#include <cstring>


namespace coda
{
    template <typename AllocatorType>
    class string_base
    {
        typedef AllocatorType allocator;
    public:

        string_base() : m_data(nullptr), m_capacity(0) {}
        string_base(const char* str);
        string_base(const string_base& other);
        string_base(string_base&& rvl);
        ~string_base();

        string_base& operator=(const string_base& other);
        string_base& operator=(string_base&& other);

        string_base& operator=(const char* str);

        bool operator==(const string_base& other) const;

        const char* c_str() const { return m_data; }
        uint32 getCapacity() const { return m_capacity; }
        uint32 getLength() const;

        void set(const char* str);
        void set(const string_base& str);
        void setFmt(const char* fmt, ...);
        void clear(bool releaseMemory = false);

    private:
        static char* allocate(uint32 size);
        static char* reallocate(char* p, uint32 size);
        static void release(void* p);

        void invalidate();

    private:
        char* m_data;
        uint32 m_capacity;
    };

    typedef string_base<baseallocator> string;

    template<typename AllocatorType>
    inline string_base<AllocatorType>::string_base(const char* str)
        : m_data(nullptr), m_capacity(0)
    {
        set(str);
    }

    template<typename AllocatorType>
    inline string_base<AllocatorType>::string_base(const string_base& other)
        : m_data(nullptr), m_capacity(0)
    {
        set(other);
    }

    template<typename AllocatorType>
    inline string_base<AllocatorType>::string_base(string_base&& rvl)
        : m_data(nullptr), m_capacity(0)
    {
        m_data = rvl.m_data;
        m_capacity = rvl.m_capacity;
        rvl.invalidate();
    }

    template<typename AllocatorType>
    inline string_base<AllocatorType>::~string_base()
    {
        clear(true);
    }

    template<typename AllocatorType>
    inline string_base<AllocatorType>& string_base<AllocatorType>::operator=(const string_base& other)
    {
        set(other);
        return *this;
    }

    template<typename AllocatorType>
    inline string_base<AllocatorType>& string_base<AllocatorType>::operator=(string_base&& rvl)
    {
        clear(true);
        m_data = rvl.m_data;
        m_capacity = rvl.m_capacity;
        rvl.invalidate();
        return *this;
    }

    template<typename AllocatorType>
    inline string_base<AllocatorType>& string_base<AllocatorType>::operator=(const char* str)
    {
        set(str);
        return *this;
    }

    template<typename AllocatorType>
    inline bool string_base<AllocatorType>::operator==(const string_base& other) const
    {
        if (!m_data && !other.m_data)
            return true;
        if (m_data && !other.m_data)
        {
            if (!*m_data) 
                return true;
            return false;
        }
        if (other.m_data && !m_data)
        {
            if (!*other.m_data)
                return true;
            return false;
        }

        return !strcmp(m_data, other.m_data);
    }

    template<typename AllocatorType>
    inline uint32 string_base<AllocatorType>::getLength() const
    {
        return m_data ? safe_cast<uint32>(strlen(m_data)) : 0;
    }

    template<typename AllocatorType>
    inline void string_base<AllocatorType>::set(const char* str)
    {
        coda_assert(str);
        uint32 s = safe_cast<uint32>(strlen(str)) + 1;
        if (m_capacity < s)
        {
            m_data = reallocate(m_data, s);
            m_capacity = s;
        }
        memcpy_s(m_data, m_capacity, str, s);
        coda_assert(m_data[s - 1] == 0);
    }

    template<typename AllocatorType>
    inline void string_base<AllocatorType>::set(const string_base& str)
    {
        set(str.c_str());
    }

    template<typename AllocatorType>
    inline void string_base<AllocatorType>::setFmt(const char* fmt, ...)
    {
        va_list va;
        va_start(va, fmt);
        uint32 s = _vscprintf(fmt, va) + 1;
        if (s > m_capacity)
        {
            m_data = reallocate(m_data, s);
            m_capacity = s;
        }
        vsprintf_s(m_data, m_capacity, fmt, va);

        va_end(va);
    }

    template<typename AllocatorType>
    inline void string_base<AllocatorType>::clear(bool releaseMemory)
    {
        if (!m_data)
            return;

        if (releaseMemory)
        {
            release(m_data);
            invalidate();
        }
        else
        {
            *m_data = 0;
        }
    }

    template<typename AllocatorType>
    inline char* string_base<AllocatorType>::allocate(uint32 size)
    {
        return (char*)allocator::allocate(size);
    }

    template<typename AllocatorType>
    inline char* string_base<AllocatorType>::reallocate(char* p, uint32 size)
    {
        return (char*)allocator::reallocate(p, size);
    }

    template<typename AllocatorType>
    inline void string_base<AllocatorType>::release(void* p)
    {
        allocator::release(p);
    }

    template<typename AllocatorType>
    inline void string_base<AllocatorType>::invalidate()
    {
        m_data = nullptr;
        m_capacity = 0;
    }
}
