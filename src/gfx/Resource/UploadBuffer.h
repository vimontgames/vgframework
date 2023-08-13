#pragma once

namespace vg::gfx
{
    class Buffer;
    class CommandList;

    class UploadBuffer
    {
    public:
        UploadBuffer(const core::string & _name, core::uint _size);
        ~UploadBuffer();

        core::u8 *      getBaseAddress  () const;
        Buffer *        getBuffer       () const;
        void            upload          (gfx::Texture * _dst, core::uint_ptr _from);
        void            upload          (gfx::Buffer * _dst, core::uint_ptr _from);
        void            flush           (CommandList * _cmdList);

        core::u8 *      map             (core::size_t _size, core::size_t _aligment);
        void            unmap           (Buffer * _buffer, core::u8 * _dst);
        void            unmap           (Texture * _texture, core::u8 * _dst);

    protected:
        core::uint_ptr  alloc           (core::size_t _size, core::size_t _alignment);

    private:
        core::uint_ptr                                          m_offsetCur = 0;
        core::uint_ptr                                          m_offsetStart = 0;
        core::mutex                                             m_uploadMutex;
        gfx::Buffer *                                           m_uploadBuffer = nullptr;
        core::u8 *                                              m_uploadBegin = nullptr;
        core::vector<core::pair<Texture*, core::uint_ptr>>      m_texturesToUpload;
        core::vector<core::pair<Buffer*, core::uint_ptr>>       m_buffersToUpload;
    };
}