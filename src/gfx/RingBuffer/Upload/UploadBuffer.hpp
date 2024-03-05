#include "UploadBuffer.h"
#include "core/Math/Math.h"
#include "gfx/CommandList/CommandList.h"
#include "gfx/Resource/Buffer.h"
#include "gfx/Profiler/Profiler.h"

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
            _aligment = max(_aligment, (size_t)32);
            return RingBuffer::map(_size, _aligment);
        }

        //--------------------------------------------------------------------------------------
        void UploadBuffer::unmap(Buffer * _buffer, core::u8 * _dst, size_t _size)
        {
            VG_ASSERT((size_t)-1 != _size);
            if (nullptr != _dst)
                upload(_buffer, _dst - getBaseAddress(), _size);
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
        void UploadBuffer::upload(gfx::Buffer * _dst, core::uint_ptr _from, size_t _size)
        {
            //VG_DEBUGPRINT("[UploadBuffer] upload buffer \"%s\" from 0x%016X\n", _dst->getName().c_str(), _from);
            VG_ASSERT((size_t)-1 != _size);
            m_buffersToUpload.push_back({ _dst, {_from, _size} });
        }

        //--------------------------------------------------------------------------------------
        void UploadBuffer::flush(CommandList * _cmdList)
        {
            lock_guard<mutex> lock(m_mutex);

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

            m_offsetStart = m_offsetCur;
        }
    }
}