namespace vg::gfx
{
    namespace base
    {
        //--------------------------------------------------------------------------------------
        VG_INLINE const BufferDesc & Buffer::getBufDesc() const
        { 
            return m_bufDesc; 
        }

        //--------------------------------------------------------------------------------------
        VG_INLINE const gfx::Resource & Buffer::getResource() const
        {
            return m_resource; 
        }

        //--------------------------------------------------------------------------------------
        VG_INLINE gfx::Resource & Buffer::getResource()
        {
            return m_resource; 
        }

        //--------------------------------------------------------------------------------------
        VG_INLINE const gfx::BindlessBufferHandle & Buffer::getBufferHandle() const
        {
            VG_ASSERT(m_bufferHandle.isValid());
            return m_bufferHandle; 
        }

        //--------------------------------------------------------------------------------------
        VG_INLINE const gfx::BindlessRWBufferHandle & Buffer::getRWBufferHandle() const
        {
            VG_ASSERT(m_rwBufferHandle.isValid());
            return m_rwBufferHandle; 
        }
    }
}