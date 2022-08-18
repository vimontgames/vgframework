#include "UploadBuffer.h"
#include "graphics/driver/CommandList/CommandList.h"

namespace vg::graphics::driver
{
    //--------------------------------------------------------------------------------------
    UploadBuffer::UploadBuffer(const core::string & _name, core::uint _size) :
        m_offsetCur(0)
    {
        BufferDesc bufDesc(
            Usage::Upload,
            BindFlags::None,
            CPUAccessFlags::Write,
            BufferFlags::None,
            _size);

        m_uploadBuffer = new driver::Buffer(bufDesc, _name);

        // keep always mapped
        Map result = m_uploadBuffer->getResource().map();
        m_uploadBegin = (core::u8*)result.data;
    }

    //--------------------------------------------------------------------------------------
    UploadBuffer::~UploadBuffer()
    {
        m_uploadBuffer->getResource().unmap();
        VG_SAFE_RELEASE(m_uploadBuffer);
    }

    //--------------------------------------------------------------------------------------
    core::u8 * UploadBuffer::map(core::size_t _size, core::size_t _aligment)
    {
        m_uploadMutex.lock();
        core::uint_ptr offset = alloc(_size, _aligment);

        if (uint_ptr(-1) != offset)
            return getBaseAddress() + offset;
        else
            return nullptr;
    }

    //--------------------------------------------------------------------------------------
    void UploadBuffer::unmap(Buffer * _buffer, core::u8 * _dst)
    {
        if (nullptr != _dst)
            upload(_buffer, _dst - getBaseAddress());
        m_uploadMutex.unlock();
    }

    //--------------------------------------------------------------------------------------
    void UploadBuffer::unmap(Texture * _texture, core::u8 * _dst)
    {
        if (nullptr != _dst)
            upload(_texture, _dst - getBaseAddress());
        m_uploadMutex.unlock();
    }

    //--------------------------------------------------------------------------------------
    core::uint_ptr UploadBuffer::alloc(size_t _size, size_t _alignment)
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

        const float size = float(alignedSize) / (1024.0f*1024.0f);
        const float available = float(max(totalSize - m_offsetCur, m_offsetStart)) / (1024.0f*1024.0f);
        const float total = float(totalSize) / (1024.0f*1024.0f);

        VG_ASSERT(false, "Cannot allocate %.2f MB for upload because ring buffer is too small (available:%.2f/%.2f MB)", size, available, total);

        return uint_ptr(-1);
    }

    //--------------------------------------------------------------------------------------
    u8 * UploadBuffer::getBaseAddress() const
    {
        return m_uploadBegin;
    }

    //--------------------------------------------------------------------------------------
    Buffer * UploadBuffer::getBuffer() const
    { 
        return m_uploadBuffer; 
    }

    //--------------------------------------------------------------------------------------
    void UploadBuffer::upload(driver::Texture * _dst, core::uint_ptr _from)
    {
        //VG_DEBUGPRINT("[UploadBuffer #%u] upload texture \"%s\" from 0x%016X\n", m_index, _dst->getName().c_str(), _from);

        m_texturesToUpload.push_back({ _dst, _from });
    }

    //--------------------------------------------------------------------------------------
    void UploadBuffer::upload(driver::Buffer * _dst, core::uint_ptr _from)
    {
        //VG_DEBUGPRINT("[UploadBuffer #%u] upload buffer \"%s\" from 0x%016X\n", m_index, _dst->getName().c_str(), _from);

        m_buffersToUpload.push_back({ _dst, _from });
    }

    //--------------------------------------------------------------------------------------
    void UploadBuffer::flush(CommandList * _cmdList)
    {
        lock_guard<mutex> lock(m_uploadMutex);

        if (m_buffersToUpload.size() || m_texturesToUpload.size())
        {
            //VG_DEBUGPRINT("[UploadBuffer #%u] Flush %u buffer(s) %u texture(s)\n", m_index, m_buffersToUpload.size(), m_texturesToUpload.size());

            for (uint i = 0; i < m_buffersToUpload.size(); ++i)
            {
                auto & pair = m_buffersToUpload[i];
                _cmdList->copyBuffer(pair.first, pair.second);
            }
            m_buffersToUpload.clear();

            for (uint i = 0; i < m_texturesToUpload.size(); ++i)
            {
                auto & pair = m_texturesToUpload[i];
                _cmdList->copyTexture(pair.first, pair.second);
            }
            m_texturesToUpload.clear();
        }

        m_index = (m_index + max_frame_latency - 1) % max_frame_latency;
        m_offsetStart = m_offsetCur;
    }
}