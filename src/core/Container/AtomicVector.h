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
        size_t size() const
        {
            return m_counter;
        }

        //--------------------------------------------------------------------------------------
        const T & operator[] (size_t _index) const
        {
            return m_data[_index];
        }

        //--------------------------------------------------------------------------------------
        // Only 'push_atomic' is lock-free, 'size' or 'clear' shouldn't be called while the vector is being filled
        //--------------------------------------------------------------------------------------
        size_t push_atomic(const T & _value)
        {
            size_t index = m_counter.fetch_add(1);
            VG_ASSERT(index < size());
            m_data[index] = _value;
            return index;
        }

    private:
        core::vector<T>         m_data;
        core::atomic<size_t>    m_counter;
    };
}