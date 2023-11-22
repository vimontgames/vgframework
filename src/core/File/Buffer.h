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

        bool read(u8 * _value);
        bool write(u8 _value);

        bool read(u16 * _value);
        bool write(u16 _value);

        bool read(u32 * _value);
        bool write(u32 _value);

        bool read(u64 * _value);
        bool write(u64 _value);

        bool read(float * _value);
        bool write(const float & _value);

        bool read(float2 * _value);
        bool write(const float2 & _value);

        bool read(float3 * _value);
        bool write(const float3 & _value);

        bool read(float4 * _value);
        bool write(const float4 & _value);

        bool read(quaternion * _value);
        bool write(const quaternion & _value);

        bool read(string * _string);
        bool write(const string & _string);

        template <typename T> bool read(vector<T> * _vector);
        template <typename T> bool write(const vector<T> & _vector);

        bool read(void * _data, size_t _size);
        bool write(const void * _data, size_t _size);

        template <typename T> bool read(T * _value);
        template <typename T> bool write(const T & _value);

        //template <typename T> bool read(core::vector<T> & _data)
        //{
        //    core::u32 size;
        //    read(&size);
        //    _data.resize(size);
        //    for (core::u32 i = 0; i < size; ++i)
        //        _data[i].read(*this);
        //}
        //
        //template <typename T> bool write(const core::vector<T> & _data)
        //{
        //    VG_ASSERT(_data.size() < (core::u32)-1);
        //    write((core::u32)_data.size());
        //    for (core::u32 i = 0; i < _data.size(); ++i)
        //        _data[i].write(*this);
        //}

    private:
        size_t              m_read = 0;
        size_t              m_write = 0;
        core::vector<u8>    m_data;
    };
}

#include "buffer.inl"



