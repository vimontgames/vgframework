#include "gfx/Precomp.h"
#include "UploadBuffer.h"
#include "gfx/CommandList/CommandList.h"
#include "gfx/Resource/Buffer.h"
#include "gfx/Profiler/Profiler.h"
#include "gfx/Device/Device.h"

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
    UploadBuffer::UploadBuffer(const string & _name, size_t _size, core::uint _index) :
        super(_name),
        m_index(_index)
    {
        BufferDesc bufDesc(Usage::Upload, BindFlags::None, CPUAccessFlags::Write, BufferFlags::None, (uint)_size);
        VG_INFO("[Upload] Create %s #%u: %u MB", _name.c_str(), _index, _size / (1024 * 1024));
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
        lock();
        _aligment = max(_aligment, minimumBufferAlignment);
        const uint_ptr offset = alloc(_size, _aligment, _category);

        VG_ASSERT(uint_ptr(-1) != offset);
        if (uint_ptr(-1) != offset)
            return getBaseAddress() + offset;

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    size_t  UploadBuffer::getAlignedSize(size_t  _size, size_t _alignment) const
    {
        _alignment = max(_alignment, minimumBufferAlignment);

        const uint_ptr baseAddress = (uint_ptr)getBaseAddress();

        // Try to fit aligned alloc at current buffer offset + alignmentOffset
        uint_ptr alignmentOffset = alignUp(baseAddress + m_currentOffset, _alignment) - (baseAddress + m_currentOffset);
        size_t alignedSize = _size + alignmentOffset;

        return alignedSize;
    }

    //--------------------------------------------------------------------------------------
    core::uint UploadBuffer::isOverlaping(const struct Alloc & _alloc, const core::vector<struct Alloc> & _ranges) const
    {
        const auto count = _ranges.size();
        for (uint i = 0; i < count; ++i)
        {
            const Alloc & range = _ranges[i];
            if (_alloc.begin < range.end && range.begin < _alloc.end)
                return i;
        }
        return -1;
    }

    //--------------------------------------------------------------------------------------
    core::u64 UploadBuffer::getTotalSize()
    {
        return m_buffer->getBufDesc().getSize();
    }

    //--------------------------------------------------------------------------------------
    void UploadBuffer::setNextAllocSize(core::size_t _size)
    {
        m_nextAllocSize = _size;
    }

    //--------------------------------------------------------------------------------------
    core::u64 UploadBuffer::getAvailableUploadSize()
    {
        Alloc dummyBestRange;
        return getAvailableUploadSize(dummyBestRange);
    }

    //--------------------------------------------------------------------------------------
    // Returns the size of the largest upload memory block
    //--------------------------------------------------------------------------------------
    core::u64 UploadBuffer::getAvailableUploadSize(Alloc & _bestRange)
    {
        Alloc bestRange(0, 0);
        u64 bestSize = 0;

        lock();
        {
            const size_t totalSize = m_buffer->getBufDesc().getSize();

            // merge and sort all ranges
            core::vector<Alloc> sortedRanges = m_previousRanges;
            for (uint i = 0; i < m_ranges.size(); ++i)
                sortedRanges.push_back(m_ranges[i]);
            sort(sortedRanges.begin(), sortedRanges.end(), [](Alloc & a, Alloc & b)
                {
                    return a.begin < b.begin;
                }
            );

            // No range? Then it's free real estate
            if (sortedRanges.size() == 0)
            {
                bestRange = Alloc(0, totalSize);
                bestSize = totalSize;
            }
            else
            {
                // How much available before first range?
                bestRange = Alloc(0, sortedRanges[0].begin);
                bestSize = sortedRanges[0].begin;

                // How much available between ranges?
                for (uint i = 0; i < sortedRanges.size() - 1; ++i)
                {
                    u64 gapStart = sortedRanges[i].end;
                    u64 gapEnd = sortedRanges[i + 1].begin;
                    u64 gapSize = gapEnd - gapStart;
                    if (gapSize > bestSize)
                    {
                        bestRange = Alloc(gapStart, gapEnd);
                        bestSize = gapSize;
                    }
                }

                // How much after last range?
                {
                    u64 gapStart = sortedRanges[sortedRanges.size() - 1].end;
                    u64 gapEnd = totalSize;
                    u64 gapSize = gapEnd - gapStart;
                    if (gapSize > bestSize)
                    {
                        bestRange = Alloc(gapStart, gapEnd);
                        bestSize = gapSize;
                    }
                }
            }
        }
        unlock();

        _bestRange = bestRange;
        return bestSize;
    }

    //--------------------------------------------------------------------------------------
    void UploadBuffer::lock()
    {
        if (m_index == 0)
            m_mutex.lock();
        else
            m_assertMutex.lock();
    }

    //--------------------------------------------------------------------------------------
    void UploadBuffer::unlock()
    {
        if (m_index == 0)
            m_mutex.unlock();
        else
            m_assertMutex.unlock();
    }

    //--------------------------------------------------------------------------------------
    bool UploadBuffer::isAvailable(core::size_t _size, core::size_t _alignment)
    {
        return tryAllocate(_size, _alignment, nullptr);
    }

    //--------------------------------------------------------------------------------------
    // TODO: Change getAvailableUploadSize to return not only largest block but also 
    // find best (smaller) fit for incoming allocation.
    //--------------------------------------------------------------------------------------
    uint_ptr UploadBuffer::alloc(size_t _size, size_t _alignment, RingAllocCategory _category)
    {
        const size_t totalSize = m_buffer->getBufDesc().getSize();
        const uint_ptr baseAddress = (uint_ptr)getBaseAddress();

        Alloc alloc;
        if (tryAllocate(_size, _alignment, &alloc))
        {
            // Check
             // Try to fit aligned alloc at current buffer offset + alignmentOffset
            uint_ptr alignmentOffset = alignUp(baseAddress + m_currentOffset, _alignment) - (baseAddress + m_currentOffset);
            size_t alignedSize = _size + alignmentOffset;
            if (m_nextAllocSize != (core::size_t)-1)
                VG_ASSERT(alignedSize == m_nextAllocSize, "[Upload] The requested upload size of %u kb (%u MB) does not match previously predicted size of %u kb (%u MB)", alignedSize, alignedSize / (1024 * 1024), m_nextAllocSize, m_nextAllocSize / (1024 * 1024));

            m_ranges.push_back(alloc);
            m_currentOffset = alloc.end;
            VG_ASSERT_IS_ALIGNED(baseAddress + alloc.begin, _alignment);
            return alloc.begin;
        }

        VG_ASSERT(false, "[Upload] Cannot allocate %u MB in upload buffer #%u", _size >> 20, m_index);
        return -1;
    }

    //--------------------------------------------------------------------------------------
    bool UploadBuffer::tryAllocate(core::size_t _size, core::size_t _alignment, Alloc * _alloc)
    {
        const size_t totalSize = m_buffer->getBufDesc().getSize();
        const uint_ptr baseAddress = (uint_ptr)getBaseAddress();

        if (m_index == 0)
        {
            // The following check is not true when multithreaded rendering is disabled
            Device * device = Device::get();
            auto jobCount = device->getMaxRenderJobCount();
            auto policy = device->getRenderJobsPolicy();
            if (jobCount > 0 )
                VG_ASSERT(Kernel::getScheduler()->IsLoadingThread()); // Upload buffer is used during init then only after for loading
        }

        // Try to fit aligned alloc at current buffer offset + alignmentOffset
        uint_ptr alignmentOffset = alignUp(baseAddress + m_currentOffset, _alignment) - (baseAddress + m_currentOffset);
        size_t alignedSize = _size + alignmentOffset;

        VG_ASSERT(alignedSize <= totalSize, "[Upload] Requested %u bytes (%u MB) but total size of %s is %u bytes (%u MB)", alignedSize, alignedSize / (1024 * 1024), GetName().c_str(), totalSize, totalSize / (1024 * 1024));
        if (alignedSize > totalSize)
            return false;

        Alloc bestRange;
        core::u64 available = getAvailableUploadSize(bestRange);
        VG_ASSERT(!_alloc || alignedSize <= available, "[Upload] Requested %u bytes (%u MB) but only %u bytes (%u/%u MB) are available in %s", alignedSize, alignedSize / (1024 * 1024), available, available / (1024 * 1024), totalSize / (1024 * 1024), GetName().c_str());
        if (alignedSize > available)
            return false;

        if ((m_currentOffset + alignedSize) < totalSize)
        {
            uint_ptr offset = m_currentOffset + alignmentOffset;

            const Alloc alloc = { offset, offset + _size };

            const auto previousFrameRangeOverlapIndex = isOverlaping(alloc, m_previousRanges);
            if (-1 != previousFrameRangeOverlapIndex)
            {
                VG_ASSERT(!_alloc || -1 == previousFrameRangeOverlapIndex, "[Upload] Could not allocated %u KB in upload buffer #%u (%u MB) because the buffer is too small for current and previous frames. Requested allocation in range [%u..%u] overlaps with previous frame allocation range [%u...%u]", (alloc.end - alloc.begin) / 1024, m_index, m_buffer->getBufDesc().getSize() / (1024*1024), alloc.begin, alloc.end, m_previousRanges[previousFrameRangeOverlapIndex].begin, m_previousRanges[previousFrameRangeOverlapIndex].end);
                if (-1 != previousFrameRangeOverlapIndex)
                    return false;
            }
            else
            {
                const auto currentFrameRangeOverlapIndex = isOverlaping(alloc, m_ranges);
                VG_ASSERT(!_alloc || -1 == currentFrameRangeOverlapIndex, "[Upload] Could not allocated %u KB in upload buffer #%u (%u MB) because the buffer is too small for current frame. Requested allocation in range [%u..%u] overlaps with previous frame allocation range [%u...%u]", (alloc.end - alloc.begin) / 1024, m_index, m_buffer->getBufDesc().getSize() >> 10, alloc.begin, alloc.end, m_ranges[currentFrameRangeOverlapIndex].begin, m_ranges[currentFrameRangeOverlapIndex].end);
                if (-1 != currentFrameRangeOverlapIndex)
                    return false;
            }

            if (nullptr != _alloc)
                *_alloc = alloc;

            VG_ASSERT_IS_ALIGNED(baseAddress + alloc.begin, _alignment);
            return true;
        }
        else 
        {
            // Try to fit at the beginning of the buffer
            alignmentOffset = alignUp(baseAddress + 0, _alignment) - (baseAddress + 0);
            alignedSize = _size + alignmentOffset;

            if (alignedSize < totalSize)
            {
                uint_ptr offset = alignmentOffset;
                VG_ASSERT(alignmentOffset == 0);

                const Alloc alloc = { offset, offset+_size };

                const auto previousFrameRangeOverlapIndex = isOverlaping(alloc, m_previousRanges);
                if (-1 != previousFrameRangeOverlapIndex)
                {
                    VG_ASSERT(!_alloc || -1 == previousFrameRangeOverlapIndex, "[Upload] Could not allocated %u KB in upload buffer #%u (%u MB) because the buffer is too small for current and previous frames. Requested allocation in range [%u..%u] overlaps with previous frame allocation range [%u...%u]", (alloc.end - alloc.begin) / 1024, m_index, m_buffer->getBufDesc().getSize() / (1024 * 1024), alloc.begin, alloc.end, m_previousRanges[previousFrameRangeOverlapIndex].begin, m_previousRanges[previousFrameRangeOverlapIndex].end);
                    if (-1 != previousFrameRangeOverlapIndex)
                        return false;
                }
                else
                {
                    const auto currentFrameRangeOverlapIndex = isOverlaping(alloc, m_ranges);
                    VG_ASSERT(!_alloc || -1 == currentFrameRangeOverlapIndex, "[Upload] Could not allocated %u KB in upload buffer #%u (%u MB) because the buffer is too small for current frame. Requested allocation in range [%u..%u] overlaps with previous frame allocation range [%u...%u]", (alloc.end - alloc.begin) / 1024, m_index, m_buffer->getBufDesc().getSize() >> 10, alloc.begin, alloc.end, m_ranges[currentFrameRangeOverlapIndex].begin, m_ranges[currentFrameRangeOverlapIndex].end);
                    if (-1 != currentFrameRangeOverlapIndex)
                        return false;
                }

                if (nullptr != _alloc)
                    *_alloc = alloc;

                VG_ASSERT_IS_ALIGNED(baseAddress + alloc.begin, _alignment);
                return true;
            }
        }

        VG_ASSERT(!_alloc, "[Upload] Cannot allocate %u MB in upload buffer #%u", _size >> 20, m_index);
        return false;
    }

    //--------------------------------------------------------------------------------------
    void UploadBuffer::unmap(Buffer * _buffer, u8 * _dst, size_t _size)
    {
        VG_ASSERT((size_t)-1 != _size);
        if (nullptr != _dst)
            upload(_buffer, _dst - getBaseAddress(), _size);
        unlock();
    }

    //--------------------------------------------------------------------------------------
    void UploadBuffer::unmap(Texture * _texture, u8 * _dst, size_t _size)
    {
        VG_ASSERT((size_t)-1 != _size);
        if (nullptr != _dst)
            upload(_texture, _dst - getBaseAddress(), _size);
        unlock();
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
        lock();
        {
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

            // Upload buffer are ready for streaming after first flush? no more necessary
            //m_readyForStreaming = true;
        }
        unlock();
    }

    //--------------------------------------------------------------------------------------
    void UploadBuffer::sync()
    {
        lock();
        {
            const uint bufferSize = m_buffer->getBufDesc().getSize();

            size_t size = 0;
            for (uint i = 0; i < m_ranges.size(); ++i)
            {
                VG_ASSERT(m_ranges[i].begin < m_ranges[i].end);
                size += m_ranges[i].end - m_ranges[i].begin;
            }

            const float ratio = (float)size / (float)bufferSize;

            if (ratio > 0.95f)
                VG_ERROR("[Upload] Buffer #%u is used at %.2f%% of its capacity (%u/%u kb)", m_index, (float(size) * 100.0f) / float(bufferSize), size >> 10, bufferSize >> 10);
            else if (ratio > 0.80f)
                VG_WARNING("[Upload] Buffer #%u is used at %.2f%% of its capacity (%u/%u kb)", m_index, (float(size) * 100.0f) / float(bufferSize), size >> 10, bufferSize >> 10);

            m_previousRanges = std::move(m_ranges);
        }
        unlock();
    }
}