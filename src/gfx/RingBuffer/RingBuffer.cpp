#include "gfx/Precomp.h"

#include "Upload/UploadBuffer.hpp"
#include "Dynamic/DynamicBuffer.hpp"

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

        m_uploadBuffer = new gfx::Buffer(bufDesc, _name);

        // keep always mapped
        Map result = m_uploadBuffer->getResource().map();
        m_uploadBegin = (core::u8 *)result.data;
    }

    //--------------------------------------------------------------------------------------
    RingBuffer::~RingBuffer()
    {
        m_uploadBuffer->getResource().unmap();
        VG_SAFE_RELEASE(m_uploadBuffer);
    }

    //--------------------------------------------------------------------------------------
    core::u8 * RingBuffer::map(core::size_t _size, core::size_t _aligment)
    {
        m_uploadMutex.lock();
        core::uint_ptr offset = alloc(_size, _aligment);

        if (uint_ptr(-1) != offset)
            return getBaseAddress() + offset;
        else
            return nullptr;
    }

    //--------------------------------------------------------------------------------------
    void RingBuffer::unmap()
    {
        m_uploadMutex.unlock();
    }

    //--------------------------------------------------------------------------------------
    core::uint_ptr RingBuffer::alloc(size_t _size, size_t _alignment)
    {
        const size_t totalSize = m_uploadBuffer->getBufDesc().size();
        const size_t alignedSize = (_size + _alignment - 1) & ~(_alignment - 1);

        if (m_offsetCur + alignedSize < totalSize)
        {
            uint_ptr offset = m_offsetCur;
            m_offsetCur += alignedSize;
            return offset;
        }
        else if (alignedSize < m_offsetStart)
        {
            m_offsetCur = 0;
            uint_ptr offset = m_offsetCur;
            m_offsetCur += alignedSize;
            return offset;
        }

        const float size = float(alignedSize) / (1024.0f * 1024.0f);
        const float available = float(max(totalSize - m_offsetCur, m_offsetStart)) / (1024.0f * 1024.0f);
        const float total = float(totalSize) / (1024.0f * 1024.0f);

        VG_ASSERT(false, "Cannot allocate %.2f MB for upload because ring buffer is too small (available:%.2f/%.2f MB)", size, available, total);

        return uint_ptr(-1);
    }

    //--------------------------------------------------------------------------------------
    u8 * RingBuffer::getBaseAddress() const
    {
        return m_uploadBegin;
    }

    //--------------------------------------------------------------------------------------
    Buffer * RingBuffer::getBuffer() const
    {
        return m_uploadBuffer;
    }
}