#pragma once

#include "gfx/RingBuffer/RingBuffer.h"

namespace vg::gfx
{
    class DynamicBuffer : public RingBuffer
    {
    public:
                        DynamicBuffer   (const core::string & _name, core::uint _size);
                        ~DynamicBuffer  ();

        //core::u8 *      map             (core::size_t _size, core::size_t _aligment);
        //void            unmap           (Buffer * _buffer, core::u8 * _dst);

        void            flush           ();

        static void     BeginFrame      ();
        static core::vector<DynamicBuffer *> s_DynamicBuffers;
    };
}