#include "UploadBuffer.h"
#include "core/Math/Math.h"
#include "gfx/CommandList/CommandList.h"
#include "gfx/Resource/Buffer.h"
#include "gfx/Resource/Texture.h"
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
        core::u8 * UploadBuffer::map(Buffer * _buffer, core::size_t _size, core::size_t _aligment)
        {
            _aligment = max(_aligment, (size_t)32);
            return RingBuffer::map(_size, _aligment, RingAllocCategory::Buffer);
        }

        //--------------------------------------------------------------------------------------
        core::u8 * UploadBuffer::map(Texture * _texture, core::size_t _size, core::size_t _aligment)
        {
            _aligment = max(_aligment, (size_t)32);
            return RingBuffer::map(_size, _aligment, RingAllocCategory::Texture);
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
        void UploadBuffer::unmap(Texture * _texture, core::u8 * _dst, size_t _size)
        {
            if (nullptr != _dst)
                upload(_texture, _dst - getBaseAddress(), _size);
            RingBuffer::unmap();
        }

        //--------------------------------------------------------------------------------------
        void UploadBuffer::upload(gfx::Texture * _dst, core::uint_ptr _from, size_t _size)
        {
            //VG_DEBUGPRINT("[UploadBuffer] upload texture \"%s\" from %u\n", _dst->GetName().c_str(), _from>>10);
            VG_ASSERT((size_t)-1 != _size);
            m_texturesToUpload.push_back({ _dst, {_from, _size} });
        }

        //--------------------------------------------------------------------------------------
        void UploadBuffer::upload(gfx::Buffer * _dst, core::uint_ptr _from, size_t _size)
        {
            //VG_DEBUGPRINT("[UploadBuffer] upload buffer \"%s\" from %u\n", _dst->GetName().c_str(), _from>>10);
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

                m_offsetStart = m_offsetCur;
            }                      
        }

        //--------------------------------------------------------------------------------------
        void UploadBuffer::sync()
        {
            lock_guard<mutex> lock(m_mutex);

            char temp[1024] = { '\0' };
            bool first = true;

            for (uint i = 0; i < core::enumCount<RingAllocCategory>(); ++i)
            {
                auto & stats = m_stats[i];

                if (stats.m_size > 1024*1024)
                {
                    if (first)
                        sprintf(temp, "Total %.2f MB (%.2f MB) / %.2f MB (", float(m_totalWriteSize) / (1024.0f * 1024.0f), float(m_totalWriteSizeAligned) / (1024.0f * 1024.0f), float(getBuffer()->getBufDesc().getSize()) / (1024.0f * 1024.0f));
                    else
                        strcat(temp, " + ");

                    first = false; 

                    sprintf(temp, "%s%u %s%s %.2f MB (%.2f MB)", temp, stats.m_count, asString((RingAllocCategory)i).c_str(), stats.m_count > 1 ? "s" : "", float(stats.m_size) / (1024.0f * 1024.0f), float(stats.m_alignedSize) / (1024.0f * 1024.0f));
                }
            }

            if (!first)
            {
                strcat(temp, ")");
                VG_INFO("[UploadBuffer] %s", temp);
            }

            m_totalWriteSize = 0;
            m_totalWriteSizeAligned = 0;
            m_padding = 0;

            for (uint i = 0; i < core::enumCount<RingAllocCategory>(); ++i)
            {
                auto & stats = m_stats[i];

                stats.m_size = 0;
                stats.m_alignedSize = 0;
                stats.m_count = 0;
            }
        }
    }
}