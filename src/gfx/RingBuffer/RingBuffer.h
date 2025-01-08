#pragma once

#include "core/Object/Object.h"
#include "core/Scheduler/Mutex.h"

namespace vg::gfx
{
    class Buffer;
    class Texture;

    vg_enum_class(RingAllocCategory, core::u8,
        Buffer = 0,
        Texture
    );

    struct RingAllocStats
    {
        core::u64  m_size = 0;
        core::u64  m_alignedSize = 0;
        core::uint m_count = 0;
    };

    class RingBuffer : public core::Object
    {
    public:
        RingBuffer(const core::string & _name, core::uint _size);
        ~RingBuffer();

        core::u8 *      getBaseAddress  () const;
        Buffer *        getBuffer       () const;

        core::u8 *      map             (core::size_t _size, core::size_t _aligment, RingAllocCategory _category);
        void            unmap           ();

        core::uint_ptr  alloc(core::size_t _size, core::size_t _alignment, RingAllocCategory _category);

    protected:
        core::uint_ptr  m_offsetCur = 0;
        core::uint_ptr  m_offsetStart = 0;
        core::u64       m_totalWriteSize = 0;
        core::u64       m_totalWriteSizeAligned = 0;
        core::u64       m_totalWritesPreviousFrame = 0;
        core::u64       m_padding = 0;
        RingAllocStats  m_stats[core::enumCount<RingAllocCategory>()];
        core::mutex     m_mutex;
        gfx::Buffer *   m_buffer = nullptr;
        core::u8 *      m_begin = nullptr;
    };
}