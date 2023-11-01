#include "UploadBuffer.h"
#include "core/Math/Math.h"
#include "gfx/CommandList/CommandList.h"
#include "gfx/Resource/Buffer.h"

namespace vg
{
    using namespace core;

    namespace gfx
    {
        //--------------------------------------------------------------------------------------
        UploadBuffer::UploadBuffer(const core::string & _name, core::uint _size) :
            RingBuffer(_name, _size)
        {
            
        }

        //--------------------------------------------------------------------------------------
        UploadBuffer::~UploadBuffer()
        {
 
        }

        //--------------------------------------------------------------------------------------
        core::u8 * UploadBuffer::map(core::size_t _size, core::size_t _aligment)
        {
            return RingBuffer::map(_size, _aligment);
        }

        //--------------------------------------------------------------------------------------
        void UploadBuffer::unmap(Buffer * _buffer, core::u8 * _dst)
        {
            if (nullptr != _dst)
                upload(_buffer, _dst - getBaseAddress());
            RingBuffer::unmap();
        }

        //--------------------------------------------------------------------------------------
        void UploadBuffer::unmap(Texture * _texture, core::u8 * _dst)
        {
            if (nullptr != _dst)
                upload(_texture, _dst - getBaseAddress());
            RingBuffer::unmap();
        }

        //--------------------------------------------------------------------------------------
        void UploadBuffer::upload(gfx::Texture * _dst, core::uint_ptr _from)
        {
            //VG_DEBUGPRINT("[UploadBuffer] upload texture \"%s\" from 0x%016X\n", _dst->getName().c_str(), _from);
            m_texturesToUpload.push_back({ _dst, _from });
        }

        //--------------------------------------------------------------------------------------
        void UploadBuffer::upload(gfx::Buffer * _dst, core::uint_ptr _from)
        {
            //VG_DEBUGPRINT("[UploadBuffer] upload buffer \"%s\" from 0x%016X\n", _dst->getName().c_str(), _from);
            m_buffersToUpload.push_back({ _dst, _from });
        }

        //--------------------------------------------------------------------------------------
        void UploadBuffer::flush(CommandList * _cmdList)
        {
            lock_guard<mutex> lock(m_mutex);

            if (m_buffersToUpload.size() || m_texturesToUpload.size())
            {
                //VG_DEBUGPRINT("[UploadBuffer] Flush %u buffer(s) %u texture(s)\n", m_buffersToUpload.size(), m_texturesToUpload.size());

                Buffer * src = getBuffer();

                for (uint i = 0; i < m_buffersToUpload.size(); ++i)
                {
                    auto & pair = m_buffersToUpload[i];
                    Buffer * dst = pair.first;
                    uint_ptr srcOffset = pair.second;
                    _cmdList->copyBuffer(dst, src, srcOffset);
                }
                m_buffersToUpload.clear();

                for (uint i = 0; i < m_texturesToUpload.size(); ++i)
                {
                    auto & pair = m_texturesToUpload[i];
                    Texture * dst = pair.first;
                    uint_ptr srcOffset = pair.second;
                    _cmdList->copyTexture(dst, src, srcOffset);
                }
                m_texturesToUpload.clear();
            }

            m_offsetStart = m_offsetCur;
        }
    }
}