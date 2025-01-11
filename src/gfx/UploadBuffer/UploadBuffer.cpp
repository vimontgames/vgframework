#include "gfx/Precomp.h"
#include "UploadBuffer.h"
#include "gfx/CommandList/CommandList.h"
#include "gfx/Resource/Buffer.h"
#include "gfx/Profiler/Profiler.h"

#if !VG_ENABLE_INLINE
#include "UploadBuffer.inl"
#endif

using namespace vg::core;

namespace vg::gfx
{
    static const size_t minimumBufferAlignment = 32;

    //--------------------------------------------------------------------------------------
    // Create an always-mapped buffer for uploads
    //--------------------------------------------------------------------------------------
    UploadBuffer::UploadBuffer(const string & _name, uint _size, core::uint _index) :
        m_index(_index)
    {
        BufferDesc bufDesc(Usage::Upload, BindFlags::None, CPUAccessFlags::Write, BufferFlags::None, _size);
        m_buffer = new Buffer(bufDesc, _name);
        Map result = m_buffer->getResource().map();
        m_begin = (u8 *)result.data;
    }

    //--------------------------------------------------------------------------------------
    // Unmap & destroy buffer
    //--------------------------------------------------------------------------------------
    UploadBuffer::~UploadBuffer()
    {
        if (nullptr != m_buffer)
        {
            m_buffer->getResource().unmap();
            VG_SAFE_RELEASE(m_buffer);
        }
    }

    //--------------------------------------------------------------------------------------
    u8 * UploadBuffer::map(Buffer * _buffer, size_t _size, size_t _aligment)
    {
        return map(_size, _aligment, RingAllocCategory::Buffer);
    }

    //--------------------------------------------------------------------------------------
    u8 * UploadBuffer::map(Texture * _buffer, size_t _size, size_t _aligment)
    {
        return map(_size, _aligment, RingAllocCategory::Texture);
    }

    //--------------------------------------------------------------------------------------
    u8 * UploadBuffer::map(size_t _size, size_t _aligment, RingAllocCategory _category)
    {
        m_mutex.lock();
        _aligment = max(_aligment, minimumBufferAlignment);
        const uint_ptr offset = alloc(_size, _aligment, _category);

        VG_ASSERT(uint_ptr(-1) != offset);
        if (uint_ptr(-1) != offset)
            return getBaseAddress() + offset;

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    bool UploadBuffer::isOverlaping(const struct Alloc & _alloc, const core::vector<struct Range> & _ranges) const
    {
        const auto count = _ranges.size();
        for (uint i = 0; i < count; ++i)
        {
            const Range & range = _ranges[i];
            if (!(_alloc.end <= range.begin || range.end <= _alloc.begin))
                return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    uint_ptr UploadBuffer::alloc(size_t _size, size_t _alignment, RingAllocCategory _category)
    {
        const size_t totalSize = m_buffer->getBufDesc().getSize();
        const uint_ptr baseAddress = (uint_ptr)getBaseAddress();

        // Try to fit aligned alloc at current buffer offset + alignmentOffset
        uint_ptr alignmentOffset = alignUp(baseAddress + m_currentOffset, _alignment) - (baseAddress + m_currentOffset);
        size_t alignedSize = _size + alignmentOffset;

        if ((m_currentOffset + alignedSize) < totalSize)
        {
            uint_ptr offset = m_currentOffset + alignmentOffset;

            if (m_ranges.size() == 0)
                m_ranges.push_back(Range(totalSize));

            const Alloc alloc = { m_currentOffset, m_currentOffset + alignedSize };

            VG_ASSERT(!isOverlaping(alloc, m_ranges),         "Could not allocated %u bytes in upload buffer %u (%u kb) because the buffer is too small for current frame. Please increase upload buffer sizes in Renderer Options > Multithreading > Render Jobs.", (alloc.end - alloc.begin), m_index, m_buffer->getBufDesc().getSize() >> 10);
            VG_ASSERT(!isOverlaping(alloc, m_previousRanges), "Could not allocated %u bytes in upload buffer %u (%u kb) because the buffer is too small for current and previous frames. Please increase upload buffer sizes in Renderer Options > Multithreading > Render Jobs.", (alloc.end - alloc.begin), m_index, m_buffer->getBufDesc().getSize() >> 10);
            
            Range & range = m_ranges[m_ranges.size() - 1];
            range.grow(alloc);

            m_currentOffset += alignedSize;
            
            VG_ASSERT_IS_ALIGNED(baseAddress + offset, _alignment);
            return offset;
        }
        else 
        {
            // Try to fit at the beginning of the buffer
            alignmentOffset = alignUp(baseAddress + 0, _alignment) - (baseAddress + 0);
            alignedSize = _size + alignmentOffset;

            if (alignedSize < totalSize)
            {
                uint_ptr offset = alignmentOffset;

                const Alloc previousAlloc = { m_currentOffset, totalSize };

                m_ranges.push_back(Range(totalSize));
                Range & previousRange = m_ranges[m_ranges.size() - 1];
                previousRange.grow(previousAlloc);

                const Alloc alloc = { 0, alignedSize };

                m_ranges.push_back(Range(totalSize));
                Range & range = m_ranges[m_ranges.size() - 1];
                range.grow(alloc);

                m_currentOffset = alignedSize;

                VG_ASSERT_IS_ALIGNED(baseAddress + offset, _alignment);
                return offset;
            }
        }

        VG_ASSERT(false, "Cannot allocate %u MB because size of upload buffer #%u is %u MB", _size >> 20, m_index, totalSize>>20);
        return -1;
    }

    //--------------------------------------------------------------------------------------
    void UploadBuffer::unmap(Buffer * _buffer, u8 * _dst, size_t _size)
    {
        VG_ASSERT((size_t)-1 != _size);
        if (nullptr != _dst)
            upload(_buffer, _dst - getBaseAddress(), _size);
        m_mutex.unlock();
    }

    //--------------------------------------------------------------------------------------
    void UploadBuffer::unmap(Texture * _texture, u8 * _dst, size_t _size)
    {
        VG_ASSERT((size_t)-1 != _size);
        if (nullptr != _dst)
            upload(_texture, _dst - getBaseAddress(), _size);
        m_mutex.unlock();
    }

    //--------------------------------------------------------------------------------------
    void UploadBuffer::upload(Texture * _dst, uint_ptr _from, size_t _size)
    {
        //VG_DEBUGPRINT("[UploadBuffer] upload texture \"%s\" from %u\n", _dst->GetName().c_str(), _from>>10);
        VG_ASSERT((size_t)-1 != _size);
        m_texturesToUpload.push_back({ _dst, {_from, _size} });
    }

    //--------------------------------------------------------------------------------------
    void UploadBuffer::upload(Buffer * _dst, core::uint_ptr _from, size_t _size)
    {   
        //VG_DEBUGPRINT("[UploadBuffer] upload buffer \"%s\" from %u\n", _dst->GetName().c_str(), _from>>10);
        VG_ASSERT((size_t)-1 != _size);
        m_buffersToUpload.push_back({ _dst, {_from, _size} });
    }

    //--------------------------------------------------------------------------------------
    void UploadBuffer::flush(CommandList * _cmdList, bool _canBeEmpty)
    {
        lock_guard lock(m_mutex);

        VG_ASSERT(_canBeEmpty || m_buffersToUpload.size() || m_texturesToUpload.size());

        if (m_buffersToUpload.size() || m_texturesToUpload.size())
        {
            VG_PROFILE_GPU_CONTEXT(_cmdList);
            VG_PROFILE_GPU("Upload");

            //VG_DEBUGPRINT("[UploadBuffer] Flush %u buffer(s) %u texture(s)\n", m_buffersToUpload.size(), m_texturesToUpload.size());

            Buffer * src = getBuffer();

            for (uint i = 0; i < m_buffersToUpload.size(); ++i)
            {
                auto & pair = m_buffersToUpload[i];
                Buffer * dst = pair.first;
                _cmdList->copyBuffer(dst, src, pair.second.offset, pair.second.size);
            }
            m_buffersToUpload.clear();

            for (uint i = 0; i < m_texturesToUpload.size(); ++i)
            {
                auto & pair = m_texturesToUpload[i];
                Texture * dst = pair.first;
                _cmdList->copyTexture(dst, src, pair.second.offset);
            }
            m_texturesToUpload.clear();
        }
    }

    //--------------------------------------------------------------------------------------
    void UploadBuffer::sync()
    {
        lock_guard lock(m_mutex);

        const uint bufferSize = m_buffer->getBufDesc().getSize();

        size_t size = 0;
        for (uint i = 0; i < m_ranges.size(); ++i)
        {
            VG_ASSERT(m_ranges[i].begin < m_ranges[i].end);
            size += m_ranges[i].end - m_ranges[i].begin;
        }
            
        if (size * 10 > bufferSize * 5)
            VG_WARNING("[Upload] Buffer #%u is used at %.2f%% of its capacity (%u/%u kb)\n", m_index, (float(size) * 100.0f) / float(bufferSize), size>>10, bufferSize>>10);

        m_previousRanges = std::move(m_ranges);
    }
}