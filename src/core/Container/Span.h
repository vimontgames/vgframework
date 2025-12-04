#pragma once

//--------------------------------------------------------------------------------------
// A replacement for std::span compatible with C++17
//--------------------------------------------------------------------------------------
namespace vg::core
{
    template <typename T> class atomicvector;

    template <typename T> class span
    {
        public:
            constexpr span() :
                m_data(nullptr),
                m_size(0) 
            {
            }

            constexpr span(T* _data, size_t _size) :
                m_data(_data),
                m_size(_size)
            {
            }

            // from vector
            constexpr span(core::vector<T> & _vector) :
                m_data(_vector.data()),
                m_size(_vector.size())
            {
            }

            constexpr span(const core::vector<T> & _vector) :
                m_data(_vector.data()),
                m_size(_vector.size())
            {
            }

            // from atomicvector
            constexpr span(const core::atomicvector<T> & _vector) :
                m_data(_vector.data()),
                m_size(_vector.size())
            {
            }

            constexpr span(core::atomicvector<T> & _vector) :
                m_data(_vector.data()),
                m_size(_vector.size())
            {
            }

            constexpr size_t size() const
            {
                return m_size;
            }

            constexpr const T & operator[](size_t _index) const 
            { 
                return m_data[_index];
            }

        private:
            const T*    m_data;
            size_t      m_size;
    };
}