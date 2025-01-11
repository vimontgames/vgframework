namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    core::u8 * UploadBuffer::getBaseAddress() const
    {
        return m_begin;
    }

    //--------------------------------------------------------------------------------------
    Buffer * UploadBuffer::getBuffer() const
    {
        return m_buffer;
    }
}