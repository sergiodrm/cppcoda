#pragma once

#include "common.h"

namespace clb
{
    template <typename T, uint32 N>
    class starray
    {
        typedef starray<T, N> self_type;
        typedef T value_type;
        typedef uint32 size_type;
    public:

        starray() : m_size(0) {}

        void clear()
        {
            resize(0);
        }

        value_type& pushBack(const value_type& value)
        {
            clb_assert(m_size < N);
            new (&m_data[(m_size++) * sizeof(value_type)]) value_type(value);
            return *reinterpret_cast<value_type*>(&m_data[(m_size - 1) * sizeof(value_type)]);
        }

        value_type& pushBack()
        {
            clb_assert(m_size < N);
            new (&m_data[(m_size++) * sizeof(value_type)]) value_type();
            return *reinterpret_cast<value_type*>(&m_data[(m_size - 1) * sizeof(value_type)]);
        }

        void resize(size_type newSize)
        {
            clb_assert(newSize <= N);
            if (newSize > m_size)
            {
                for (size_type i = m_size; i < newSize; ++i)
                    new (&m_data[i * sizeof(value_type)]) value_type();
            }
            else
            {
                for (size_type i = newSize; i < m_size; ++i)
                    reinterpret_cast<value_type*>(&m_data[i * sizeof(value_type)])->~value_type();
            }
            m_size = newSize;
        }

        void popBack()
        {
            clb_assert(m_size > 0);
            --m_size;
            reinterpret_cast<value_type*>(&m_data[m_size * sizeof(value_type)])->~value_type();
        }

        value_type& getBack()
        {
            clb_assert(m_size > 0);
            return *reinterpret_cast<value_type*>(&m_data[(m_size - 1) * sizeof(value_type)]);
        }

        const value_type& getBack() const
        {
            clb_assert(m_size > 0);
            return *reinterpret_cast<const value_type*>(&m_data[(m_size - 1) * sizeof(value_type)]);
        }

        bool isEmpty() const { return m_size == 0; }
        size_type getSize() const { return m_size; }
        static constexpr size_type getCapacity() { return N; }

        value_type* getData() { return reinterpret_cast<value_type*>(m_data); }
        const value_type* getData() const { return reinterpret_cast<const value_type*>(m_data); }
        bool isValidIndex(size_type index) const { return index < m_size; }

        const value_type& operator[](size_type index) const
        {
            clb_assert(index < m_size);
            return *reinterpret_cast<const value_type*>(&m_data[index * sizeof(value_type)]);
        }

    private:
        byte m_data[N*sizeof(value_type)];
        size_type m_size;
    };
}