#pragma once

#include "gfx/RingBuffer/RingBuffer.h"

namespace vg::gfx
{
    class DynamicBuffer : public RingBuffer
    {
    public:
        DynamicBuffer(const core::string & _name, core::uint _size);
        ~DynamicBuffer();

        void flush();

        static void Flush();
        static core::vector<DynamicBuffer *> s_DynamicBuffers;
    };
}