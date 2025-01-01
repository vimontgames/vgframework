#include "gfx/Precomp.h"

#include "Upload/UploadBuffer.hpp"

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    RingBuffer::RingBuffer(const core::string & _name, core::uint _size) :
        m_offsetCur(0)
    {
        BufferDesc bufDesc(
            Usage::Upload,
            BindFlags::None,
            CPUAccessFlags::Write,
            BufferFlags::None,
            _size);

        m_buffer = new gfx::Buffer(bufDesc, _name);

        // keep always mapped
        Map result = m_buffer->getResource().map();
        m_begin = (core::u8 *)result.data;
    }

    //--------------------------------------------------------------------------------------
    RingBuffer::~RingBuffer()
    {
        m_buffer->getResource().unmap();
        VG_SAFE_RELEASE(m_buffer);
    }

    //--------------------------------------------------------------------------------------
    core::u8 * RingBuffer::map(core::size_t _size, core::size_t _aligment, RingAllocCategory _category)
    {
        m_mutex.lock();
        core::uint_ptr offset = alloc(_size, _aligment, _category);

        if (uint_ptr(-1) != offset)
            return getBaseAddress() + offset;

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    void RingBuffer::unmap()
    {
        m_mutex.unlock();
    }

    //--------------------------------------------------------------------------------------
    // Not only the allocation size but also the allocation start address must be aligned as requested
    //--------------------------------------------------------------------------------------
    core::uint_ptr RingBuffer::alloc(size_t _size, size_t _alignment, RingAllocCategory _category)
    {
        const size_t totalSize = m_buffer->getBufDesc().getSize();       

        const uint_ptr baseAddress = (uint_ptr)getBaseAddress();
        uint_ptr alignedStartOffset = alignUp(baseAddress + m_offsetCur, _alignment) - (baseAddress + m_offsetCur);
        size_t alignedSize = alignUp(_size, _alignment) + alignedStartOffset;

        auto & stats = m_stats[(uint)_category];

        if ((m_totalWriteSizeAligned + alignedSize) < totalSize)
        {
            if (m_offsetCur + alignedSize < totalSize)
            {
                uint_ptr offset = m_offsetCur;
                m_offsetCur += alignedSize;

                m_totalWriteSize += _size;
                m_totalWriteSizeAligned += alignedSize;
                
                stats.m_size += _size;
                stats.m_alignedSize += alignedSize;
                stats.m_count++;

                VG_ASSERT_IS_ALIGNED(baseAddress + offset + alignedStartOffset, _alignment);
                return offset + alignedStartOffset;
            }
            else
            {
                // Recompute new aligned size from the start of the buffer
                alignedStartOffset = alignUp(baseAddress + 0, _alignment) - (baseAddress + 0);
                alignedSize = alignUp(_size, _alignment) + alignedStartOffset;

                if (alignedSize < m_offsetStart)
                {
                    m_padding = totalSize - m_offsetCur;
                    m_offsetCur = 0;

                    m_totalWriteSize += _size;
                    m_totalWriteSizeAligned += alignedSize + m_padding;

                    uint_ptr offset = m_offsetCur;
                    m_offsetCur += alignedSize;

                    stats.m_size += _size;
                    stats.m_alignedSize += alignedSize;
                    stats.m_count++;

                    VG_ASSERT_IS_ALIGNED(baseAddress + offset + alignedStartOffset, _alignment);
                    return offset + alignedStartOffset;
                }
            }
        }

        const float BytesToMegabytes = 1.0f / (1024.0f * 1024.0f);

        const float size = float(alignedSize) * BytesToMegabytes;
        const float available = float(totalSize - m_totalWriteSizeAligned) * BytesToMegabytes;
        const float total = float(totalSize) * BytesToMegabytes;
        const float pad = float(m_padding) * BytesToMegabytes;

        VG_ASSERT(false, "Cannot allocate %.2f MB in upload ring buffer because there is only %.2f/%.2f MB available", size, available, total);

        return uint_ptr(-1);
    }

    //--------------------------------------------------------------------------------------
    u8 * RingBuffer::getBaseAddress() const
    {
        return m_begin;
    }

    //--------------------------------------------------------------------------------------
    Buffer * RingBuffer::getBuffer() const
    {
        return m_buffer;
    }
}