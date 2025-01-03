#pragma once

#include "gfx/RingBuffer/RingBuffer.h"
#include "gfx/Resource/Resource.h"

namespace vg::gfx
{
    class CommandList;

    class UploadBuffer : public RingBuffer
    {
    public:
        UploadBuffer(const core::string & _name, core::uint _size);
        ~UploadBuffer();

        core::u8 *      map             (Buffer * _buffer, core::size_t _size, core::size_t _aligment);
        core::u8 *      map             (Texture * _buffer, core::size_t _size, core::size_t _aligment);

        void            unmap           (Buffer * _buffer, core::u8 * _dst, size_t _size);
        void            unmap           (Texture * _texture, core::u8 * _dst, size_t _size);

        void            upload          (gfx::Texture * _dst, core::uint_ptr _from, size_t _size);
        void            upload          (gfx::Buffer * _dst, core::uint_ptr _from, size_t _size);

        void            flush           (CommandList * _cmdList, bool _canBeEmpty = false);
        void            sync            ();

    private:
        struct ResourceUploadInfo
        {
            ResourceUploadInfo(core::uint_ptr _offset, size_t _size = (size_t) - 1) :
                offset(_offset),
                size(_size)
            {

            }
            core::uint_ptr offset;
            size_t size;
        };

        core::vector<core::pair<Texture*, ResourceUploadInfo>>  m_texturesToUpload;
        core::vector<core::pair<Buffer*, ResourceUploadInfo>>   m_buffersToUpload;
    };
}