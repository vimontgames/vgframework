#pragma once

#include "core/Types/Types.h"

namespace vg::core::io
{
    class Buffer
    {
    public:
        Buffer(const u8 * _data = nullptr, size_t _size = 0);

        const u8 * data() const;
        size_t size() const;
        void resize(size_t _size);

        bool read(u32 * _value);
        bool write(u32 _value);

        bool read(string * _string);
        bool write(const string & _string);

        template <typename T> bool read(vector<T> * _vector);
        template <typename T> bool write(const vector<T> & _vector);

        bool read(void * _data, size_t _size);
        bool write(const void * _data, size_t _size);

    private:
        size_t              m_read = 0;
        size_t              m_write = 0;
        core::vector<u8>    m_data;
    };
}

#include "buffer.inl"



