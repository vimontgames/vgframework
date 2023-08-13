#pragma once

#include "gfx/RingBuffer/RingBuffer.h"

namespace vg::gfx
{
    class CommandList;

    class UploadBuffer : public RingBuffer
    {
    public:
        UploadBuffer(const core::string & _name, core::uint _size);
        ~UploadBuffer();

        core::u8 *      map             (core::size_t _size, core::size_t _aligment);
        void            unmap           (Buffer * _buffer, core::u8 * _dst);
        void            unmap           (Texture * _texture, core::u8 * _dst);

        void            upload          (gfx::Texture * _dst, core::uint_ptr _from);
        void            upload          (gfx::Buffer * _dst, core::uint_ptr _from);

        void            flush           (CommandList * _cmdList);

    private:
        core::vector<core::pair<Texture*, core::uint_ptr>>      m_texturesToUpload;
        core::vector<core::pair<Buffer*, core::uint_ptr>>       m_buffersToUpload;
    };
}