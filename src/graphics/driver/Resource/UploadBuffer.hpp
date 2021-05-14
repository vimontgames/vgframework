#include "UploadBuffer.h"
#include "graphics/driver/CommandList/CommandList.h"

namespace vg::graphics::driver
{
    //--------------------------------------------------------------------------------------
    UploadBuffer::UploadBuffer(const core::string & _name, core::uint _size)
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
        m_offsetCur = 0;
    }

    //--------------------------------------------------------------------------------------
    UploadBuffer::~UploadBuffer()
    {
        m_uploadBuffer->getResource().unmap();
        VG_SAFE_RELEASE(m_uploadBuffer);
    }

    //--------------------------------------------------------------------------------------
    core::uint_ptr UploadBuffer::alloc(size_t _size, size_t _alignment)
    {
        size_t alignedSize = (_size + _alignment - 1) & ~(_alignment - 1);
        uint_ptr offset = m_offsetCur.fetch_add(alignedSize);
        VG_ASSERT(offset + alignedSize < m_uploadBuffer->getBufDesc().size(), "Cannot allocate %.2f MB in upload buffer heap (upload heap size is %.2f MB)", float(alignedSize) / (1024.0f*1024.0f), float(m_uploadBuffer->getBufDesc().size()) / (1024.0f*1024.0f));
        return offset;
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
        m_texturesToUpload.push_back({ _dst, _from });
    }

    //--------------------------------------------------------------------------------------
    void UploadBuffer::upload(driver::Buffer * _dst, core::uint_ptr _from)
    {
        m_buffersToUpload.push_back({ _dst, _from });
    }

    //--------------------------------------------------------------------------------------
    void UploadBuffer::flush(CommandList * _cmdList)
    {
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

        m_offsetCur = 0;
    }
}