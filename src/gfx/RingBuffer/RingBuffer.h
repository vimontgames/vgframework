#pragma once

#include "core/Object/Object.h"

namespace vg::gfx
{
    class Buffer;
    class Texture;

    class RingBuffer : public core::Object
    {
    public:
        RingBuffer(const core::string & _name, core::uint _size);
        ~RingBuffer();

        core::u8 *      getBaseAddress  () const;
        Buffer *        getBuffer       () const;

        core::u8 *      map             (core::size_t _size, core::size_t _aligment);
        void            unmap           ();

        core::uint_ptr  alloc(core::size_t _size, core::size_t _alignment);

    protected:
        core::uint_ptr  m_offsetCur = 0;
        core::uint_ptr  m_offsetStart = 0;
        core::mutex     m_uploadMutex;
        gfx::Buffer *   m_uploadBuffer = nullptr;
        core::u8 *      m_uploadBegin = nullptr;
    };
}