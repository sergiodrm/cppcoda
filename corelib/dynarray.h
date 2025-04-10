#pragma once

#include "common.h"

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
            resize(capacity);
        }
        ~dynarray() { clear(true); }

        void setIncrementFactor(float factor = defaultIncrementFactor) { m_incrementFactor = factor; }

        void resize(size_type newCapacity)
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
                        static_cast<value_type*>(&m_data[i * sizeof(value_type)])->~value_type();
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
                m_data[i].~value_type();
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
            new (&m_data[m_size++]) value_type(value);
            return m_data[m_size-1];
        }

        value_type& pushBack()
        {
            return pushBack(value_type());
        }

        bool isEmpty() const { return m_size == 0; }
        value_type* getData() { return m_data; }
        const value_type* getData() const { return m_data; }

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