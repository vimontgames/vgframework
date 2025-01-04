#pragma once

#include "Types.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // 2D vector: Use my2DVector[x][y] to access elements
    //--------------------------------------------------------------------------------------
    template <typename T> class vector2D
    {
    public:
        vector2D(size_t _width, size_t _height, T _initValue) :
            m_width(_width),
            m_height(_height),
            m_data(_width * _height)
        {
            for (uint i = 0; i < m_width * m_height; ++i)
                m_data[i] = _initValue;
        }

        class ColumnProxy 
        {
            public:
                ColumnProxy(T * _data, size_t _x, size_t _width, size_t _height) : 
                    m_data(_data), 
                    m_x(_x),
                    m_width(_width),
                    m_height(_height)
                {
                }

                T & operator[](size_t y) 
                {
                    VG_ASSERT(y < m_height);
                    return const_cast<T&>(m_data[y * m_width + m_x]);
                }

                const T & operator[](size_t y) const
                {
                    VG_ASSERT(y < m_height);
                    return m_data[y * m_width + m_x];
                }

            private:
                const T *   m_data = nullptr;
                size_t      m_x = 0;
                size_t      m_width = 0;
                size_t      m_height = 0;
        };

        ColumnProxy operator[](size_t x)
        {
            VG_ASSERT(x < m_width);
            return ColumnProxy(m_data.data(), x, m_width, m_height);
        }

        const ColumnProxy operator[](size_t x) const
        {
            VG_ASSERT(x < m_width);
            return ColumnProxy(m_data.data(), x, m_width, m_height);
        }

    private:
        size_t m_width = 0;
        size_t m_height = 0;
        core::vector<T> m_data;
    };
}