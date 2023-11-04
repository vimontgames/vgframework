namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    // FrameGraph::Resource
    //--------------------------------------------------------------------------------------
    FrameGraphResource::FrameGraphResource()
    {

    }

    //--------------------------------------------------------------------------------------
    void FrameGraphResource::setType(FrameGraphResource::Type _type)
    {
        m_type = _type;
    }

    //--------------------------------------------------------------------------------------
    FrameGraphResource::Type FrameGraphResource::getType() const
    {
        return m_type;
    }

    //--------------------------------------------------------------------------------------
    void FrameGraphResource::setName(const core::string & _name)
    {
        m_name = _name;
    }

    //--------------------------------------------------------------------------------------
    const core::string & FrameGraphResource::getName() const
    {
        return m_name;
    }

    //--------------------------------------------------------------------------------------
    void FrameGraphResource::setReadAtPass(const UserPass * _subPass)
    {
        m_read.push_back(_subPass);
        m_readWrite.push_back(PassRWAccess(_subPass, RWFlags::Read));
    }

    //--------------------------------------------------------------------------------------
    const core::vector<const UserPass *> & FrameGraphResource::getReadAtPass() const
    {
        return m_read;
    }

    //--------------------------------------------------------------------------------------
    void FrameGraphResource::setWriteAtPass(const UserPass * _subPass)
    {
        m_write.push_back(_subPass);
        m_readWrite.push_back(PassRWAccess(_subPass, RWFlags::Write));
    }

    //--------------------------------------------------------------------------------------
    void FrameGraphResource::setReadWriteAtPass(const UserPass * _subPass)
    {
        m_read.push_back(_subPass);
        m_write.push_back(_subPass);
        m_readWrite.push_back(PassRWAccess(_subPass, RWFlags::Read | RWFlags::Write));
    }

    //--------------------------------------------------------------------------------------
    const core::vector<const UserPass *> & FrameGraphResource::getWriteAtPass() const
    {
        return m_write;
    }

    //--------------------------------------------------------------------------------------
    const core::vector<FrameGraphResource::PassRWAccess> & FrameGraphResource::getReadWriteAccess() const
    {
        return m_readWrite;
    }

    //--------------------------------------------------------------------------------------
    void FrameGraphResource::setCurrentState(ResourceState _state)
    {
        m_state = _state;
    }

    //--------------------------------------------------------------------------------------
    ResourceState FrameGraphResource::getCurrentState() const
    {
        return m_state;
    }

    //--------------------------------------------------------------------------------------
    // FrameGraphTextureResource
    //--------------------------------------------------------------------------------------
    void FrameGraphTextureResource::setTextureResourceDesc(const FrameGraphTextureResourceDesc & _texResDesc)
    {
        m_desc = _texResDesc;
    }

    //--------------------------------------------------------------------------------------
    void FrameGraphTextureResource::setTexture(Texture * _tex)
    {
        VG_ASSERT(nullptr == m_texture, "Resource \"%s\" already has a texture", getName().c_str());
        m_texture = _tex;
    }

    //--------------------------------------------------------------------------------------
    void FrameGraphTextureResource::resetTexture()
    {
        VG_ASSERT(m_desc.transient);
        m_texture = nullptr;
    }

    //--------------------------------------------------------------------------------------
    const FrameGraphTextureResourceDesc & FrameGraphTextureResource::getTextureResourceDesc() const
    {
        return m_desc;
    }

    //--------------------------------------------------------------------------------------
    Texture * FrameGraphTextureResource::getTexture() const
    {
        VG_ASSERT(nullptr != m_texture, "Resource \"%s\" has not texture", getName().c_str());
        return m_texture;
    }

    //--------------------------------------------------------------------------------------
    // FrameGraph::BufferResource
    //--------------------------------------------------------------------------------------
    void FrameGraphBufferResource::setBufferResourceDesc(const FrameGraphBufferResourceDesc & _bufResDesc, Buffer * _buffer)
    {
        m_desc = _bufResDesc;
        m_buffer = _buffer;
    }

    //--------------------------------------------------------------------------------------
    const FrameGraphBufferResourceDesc & FrameGraphBufferResource::getBufferResourceDesc() const
    {
        return m_desc;
    }

    //--------------------------------------------------------------------------------------
    Buffer * FrameGraphBufferResource::getBuffer() const
    {
        return m_buffer;
    }
}