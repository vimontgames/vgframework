#pragma once

namespace vg::graphics::driver
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
        void            upload          (driver::Texture * _dst, core::uint_ptr _from);
        void            upload          (driver::Buffer * _dst, core::uint_ptr _from);
        void            flush           (CommandList * _cmdList);

        core::u8 *      map             (core::size_t _size, core::size_t _aligment);
        void            unmap           (Buffer * _buffer, core::u8 * _dst);
        void            unmap           (Texture * _texture, core::u8 * _dst);

    protected:
        core::uint_ptr  alloc(core::size_t _size, core::size_t _alignment);

    private:
        core::mutex                                             m_uploadMutex;
        driver::Buffer *                                        m_uploadBuffer = nullptr;
        core::u8 *                                              m_uploadBegin = nullptr;
        core::atomic<core::uint_ptr>                            m_offsetCur = 0;
        core::vector<core::pair<Texture*, core::uint_ptr>>      m_texturesToUpload;
        core::vector<core::pair<Buffer*, core::uint_ptr>>       m_buffersToUpload;
    };
}