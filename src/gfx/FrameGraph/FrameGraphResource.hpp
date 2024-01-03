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
        m_readWrite.push_back(PassRWAccess(_subPass, RWFlags::Read));
    }

    //--------------------------------------------------------------------------------------
    void FrameGraphResource::setWriteAtPass(const UserPass * _subPass)
    {
        m_readWrite.push_back(PassRWAccess(_subPass, RWFlags::Write));
    }

    //--------------------------------------------------------------------------------------
    void FrameGraphResource::setReadWriteAtPass(const UserPass * _subPass)
    {
        m_readWrite.push_back(PassRWAccess(_subPass, RWFlags::Read | RWFlags::Write));
    }

    //--------------------------------------------------------------------------------------
    bool FrameGraphResource::isRead(const UserPass * _userPass) const
    {
        for (uint i = 0; i < m_readWrite.size(); ++i)
        {
            const PassRWAccess & readWrite = m_readWrite[i];

            if (_userPass == readWrite.m_userPass)
            {
                if (asBool(RWFlags::Read & readWrite.m_rwAccess))
                    return true;
                break;
            }            
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool FrameGraphResource::isFirstRead(const UserPass * _userPass) const
    {
        for (uint i = 0; i < m_readWrite.size(); ++i)
        {
            const PassRWAccess & readWrite = m_readWrite[i];
            if (asBool(RWFlags::Read & readWrite.m_rwAccess))
            {
                if (_userPass == readWrite.m_userPass)
                    return true;
                break;
            }
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool FrameGraphResource::isLastRead(const UserPass * _userPass) const
    {
        if (m_readWrite.size() > 0)
        {
            for (int i = (int)m_readWrite.size() - 1; i >= 0; --i)
            {
                const PassRWAccess & readWrite = m_readWrite[i];
                if (asBool(RWFlags::Read & readWrite.m_rwAccess))
                {
                    if (_userPass == readWrite.m_userPass)
                        return true;
                    break;
                }
            }
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool FrameGraphResource::isWrite(const UserPass * _userPass) const
    {
        for (uint i = 0; i < m_readWrite.size(); ++i)
        {
            const PassRWAccess & readWrite = m_readWrite[i];

            if (_userPass == readWrite.m_userPass)
            {
                if (asBool(RWFlags::Write & readWrite.m_rwAccess))
                    return true;
                break;
            }
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool FrameGraphResource::isFirstWrite(const UserPass * _userPass) const
    {
        for (uint i = 0; i < m_readWrite.size(); ++i)
        {
            const PassRWAccess & readWrite = m_readWrite[i];
            if (asBool(RWFlags::Write & readWrite.m_rwAccess))
            {
                if (_userPass == readWrite.m_userPass)
                    return true;
                break;
            }
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool FrameGraphResource::isLastWrite(const UserPass * _userPass) const
    {
        if (m_readWrite.size() > 0)
        {
            for (int i = (int)m_readWrite.size() - 1; i >= 0; --i)
            {
                const PassRWAccess & readWrite = m_readWrite[i];
                if (asBool(RWFlags::Write & readWrite.m_rwAccess))
                {
                    if (_userPass == readWrite.m_userPass)
                        return true;
                    break;
                }
            }
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool FrameGraphResource::isReadAfter(const UserPass * _userPass) const
    {
        if (m_readWrite.size() > 0)
        {
            bool read = false;
            for (int i = (int)m_readWrite.size() - 1; i >= 0; --i)
            {
                const PassRWAccess & readWrite = m_readWrite[i];

                if (!read && asBool(RWFlags::Read & readWrite.m_rwAccess))
                {
                    read = true;
                }

                if (asBool(RWFlags::Write & readWrite.m_rwAccess))
                {
                    if (_userPass == readWrite.m_userPass)
                        return read;
                    break;
                }
            }
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    // Previous state was 'read' and new state is 'write'
    //--------------------------------------------------------------------------------------
    bool FrameGraphResource::needsReadToWriteTransition(const UserPass * _userPass) const
    {
        bool previousRead = false;
        bool previousWrite = false;

        for (uint i = 0; i < m_readWrite.size(); ++i)
        {
            const PassRWAccess & readWrite = m_readWrite[i];

            const bool read = asBool(RWFlags::Read & readWrite.m_rwAccess);
            const bool write = asBool(RWFlags::Write & readWrite.m_rwAccess);

            if (_userPass == readWrite.m_userPass)
            {
                if (previousRead && write)
                    return true;
                break;
            }

            previousRead = read;
            previousWrite = write;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool FrameGraphResource::needsWriteToReadTransition(const UserPass * _userPass) const
    {
        bool previousRead = false;
        bool previousWrite = false;

        for (uint i = 0; i < m_readWrite.size(); ++i)
        {
            const PassRWAccess & readWrite = m_readWrite[i];

            const bool read = asBool(RWFlags::Read & readWrite.m_rwAccess);
            const bool write = asBool(RWFlags::Write & readWrite.m_rwAccess);

            if (_userPass == readWrite.m_userPass)
            {
                if (previousWrite && read)
                    return true;
                break;
            }

            previousRead = read;
            previousWrite = write;
        }

        return false;
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
        VG_ASSERT(nullptr != m_texture, "Resource \"%s\" has no texture", getName().c_str());
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
    void FrameGraphBufferResource::setBuffer(Buffer * _buffer)
    {
        VG_ASSERT(nullptr == m_buffer, "Resource \"%s\" already has a buffer", getName().c_str());
        m_buffer = _buffer;
    }

    //--------------------------------------------------------------------------------------
    Buffer * FrameGraphBufferResource::getBuffer() const
    {
        return m_buffer;
    }

    //--------------------------------------------------------------------------------------
    void FrameGraphBufferResource::resetBuffer()
    {
        VG_ASSERT(m_desc.transient);
        m_buffer = nullptr;
    }
}