#pragma once

#include "common.h"
#include <cmath>

namespace clb
{
    template <typename T>
    class dynarray
    {
        typedef dynarray<T> self_type;
        typedef T value_type;
        typedef uint32 size_type;
    public:
        static constexpr float defaultIncrementFactor = 1.5f;

        dynarray() : m_data(nullptr), m_size(0), m_capacity(0), m_incrementFactor(defaultIncrementFactor) {}
        dynarray(size_type capacity) : m_data(nullptr), m_size(0), m_capacity(0), m_incrementFactor(defaultIncrementFactor)
        {
            reserve(capacity);
        }
        ~dynarray() { clear(true); }

        void setIncrementFactor(float factor = defaultIncrementFactor) { m_incrementFactor = factor; }

        void reserve(size_type newCapacity)
        {
            // same as current capacity
            if (newCapacity == m_capacity) 
                return;

            if (newCapacity == 0)
            {
                clear(true);
                return;
            }

            // capacity is less than currently allocated
            if (newCapacity < m_capacity)
            {
                // destruct the elements between current size and new capacity
                if (m_size >= newCapacity)
                {
                    for (size_type i = newCapacity; i < m_size; ++i)
                        destroyItem(i);
                    m_size = newCapacity;
                    shrink();
                }
                else
                {
                    m_data = reallocate(m_data, newCapacity);
                    m_capacity = newCapacity;
                }
            }
            else
            {
                // capacity is greater than currently allocated
                m_data = reallocate(m_data, newCapacity);
                m_capacity = newCapacity;
            }
        }

        void resize(size_type newSize)
        {
            if (newSize == m_size)
                return;
            
            if (newSize > m_size)
            {
                if (newSize > m_capacity)
                    reserve(newSize);
                m_size = newSize;
            }
            else
            {
                for (uint32 i = newSize; i < m_size; ++i)
                    destroyItem(i);
                m_size = newSize;
                shrink();
            }
        }

        void fill(const value_type& value, size_type first, size_type count)
        {
            size_type end = first + count;
            clb_assert(end <= m_size);
            for (size_type i = first; i < end; ++i)
                constructItem(i, value);
        }

        void shrink()
        {
            if (!m_capacity || m_size == m_capacity)
                return;
            value_type* newData = reallocate(m_data, m_size);
            m_data = newData;
            m_capacity = m_size;
        }

        void clear(bool releaseMemory = false)
        {
            for (size_type i = 0; i < m_size; ++i)
                destroyItem(i);
            m_size = 0;
            if (releaseMemory)
                shrink();
        }

        value_type& pushBack(const value_type& value)
        {
            clb_assert(m_size <= m_capacity && m_incrementFactor > 1.f);
            if (m_size == m_capacity)
            {
                size_type newCapacity = static_cast<size_type>(ceilf((float)m_capacity * m_incrementFactor));
                clb_assert(newCapacity > m_capacity);
                resize(newCapacity);
            }
            constructItem(m_size++, value);
            return m_data[m_size-1];
        }

        value_type& pushBack()
        {
            return pushBack(value_type());
        }

        bool isEmpty() const { return m_size == 0; }
        size_type getSize() const { return m_size; }
        size_type getCapacity() const { return m_capacity; }
        value_type* getData() { return m_data; }
        const value_type* getData() const { return m_data; }
        bool isValidIndex(size_type index) { return index < m_size; }

        value_type& operator[](size_type index)
        {
            clb_assert(index < m_size);
            return &m_data[index];
        }

        const value_type& operator[](size_type index) const
        {
            clb_assert(index < m_size);
            return &m_data[index];
        }

    private:

        void constructItem(size_type index, const value_type& value)
        {
            new(&m_data[index])value_type(value);
        }

        void destroyItem(size_type index)
        {
            m_data[index].~value_type();
        }

        static value_type* allocate(size_type count)
        {
            value_type* data = (value_type*)malloc(count * sizeof(value_type));
            clb_assert(data != nullptr);
            return data;
        }

        static value_type* reallocate(value_type* data, size_type count)
        {
            if (data == nullptr)
                return allocate(count);
            if (count == 0)
            {
                release(data);
                return nullptr;
            }
            value_type* newData = (value_type*)realloc(data, count * sizeof(value_type));
            clb_assert(newData != nullptr);
            return newData;
        }

        static void release(value_type* data)
        {
            clb_assert(data != nullptr);
            free(data);
        }

    private:
        value_type* m_data;
        size_type m_size;
        size_type m_capacity;
        float m_incrementFactor;
    };
}