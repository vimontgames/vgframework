#pragma once

#ifdef VG_DEBUG
// Clearing vector contents can be slow with large vectors, and is not strictly necessary, so do it only for debug builds
#define VG_ATOMIC_VECTOR_DEBUG_CLEAR_PATTERN 0xAA 
#endif

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
            if (0 != m_counter.exchange(0))
            {
                #if VG_ATOMIC_VECTOR_DEBUG_CLEAR_PATTERN
                VG_PROFILE_CPU("atomicvector::clear");
                memset(m_data.data(), VG_ATOMIC_VECTOR_DEBUG_CLEAR_PATTERN, m_data.size() * sizeof(T));
                #endif
            }
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
        inline const T * data() const
        {
            return m_data.data();
        }

        //--------------------------------------------------------------------------------------
        inline const T & operator[] (size_t _index) const
        {
            return m_data.data()[_index];
        }

        //--------------------------------------------------------------------------------------
        inline T & operator[] (size_t _index)
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
            VG_ASSERT(index < capacity(), "Cannot add element to atomic vector (size: %u)", m_data.size());
            return m_data.data()[index];
        }

        //--------------------------------------------------------------------------------------
        inline T * alloc(size_t _count)
        {
            size_t offset = m_counter.fetch_add(_count);
            VG_ASSERT(offset < capacity(), "Cannot allocated %u elements in atomic vector (size: %u)", _count, m_data.size());
            return &m_data.data()[offset];
        }

    private:
        core::vector<T>         m_data;
        core::atomic<size_t>    m_counter;
    };
}