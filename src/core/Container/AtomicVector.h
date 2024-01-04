#pragma once

namespace vg::core
{
    template <typename T> class atomicvector
    {
    public:
        //--------------------------------------------------------------------------------------
        atomicvector(size_t _size) : 
            m_data(_size),
            m_counter(0)
        {

        }

        //--------------------------------------------------------------------------------------
        void clear()
        {
            memset(m_data.data(), 0x0, m_data.size() * sizeof(T));
            m_counter = 0;
        }

        //--------------------------------------------------------------------------------------
        inline size_t capacity()
        {
            return m_data.capacity();
        }

        //--------------------------------------------------------------------------------------
        inline size_t size() const
        {
            return m_counter;
        }

        //--------------------------------------------------------------------------------------
        inline const T & operator[] (size_t _index) const
        {
            return m_data.data()[_index];
        }

        //--------------------------------------------------------------------------------------
        // Only 'push_atomic' is lock-free, 'size' or 'clear' shouldn't be called while the vector is being filled
        //--------------------------------------------------------------------------------------
        inline size_t push_back_atomic(const T & _value)
        {
            size_t index = m_counter.fetch_add(1);
            VG_ASSERT(index < capacity());
            m_data.data()[index] = _value;
            return index;
        }

        //--------------------------------------------------------------------------------------
        inline T & push_empty_atomic()
        {
            size_t index = m_counter.fetch_add(1);
            VG_ASSERT(index < capacity());
            return m_data.data()[index];
        }

        //--------------------------------------------------------------------------------------
        inline T * alloc(size_t _count)
        {
            size_t offset = m_counter.fetch_add(_count);
            VG_ASSERT(offset < capacity());
            return &m_data.data()[offset];
        }

    private:
        core::vector<T>         m_data;
        core::atomic<size_t>    m_counter;
    };
}