namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    VG_INLINE const core::IObject * ResourceInfo::getObject() const
    {
        return m_object;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::IObject * ResourceInfo::getObject()
    {
        return m_object;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::IObject *& ResourceInfo::getObjectRef()
    {
        return m_object;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const core::string & ResourceInfo::getPath() const
    {
        return m_path;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE const core::vector<core::IResource *> & ResourceInfo::getClients() const
    {
        return m_clients;
    }

    #if USE_CLIENTS_TO_UPDATE_LIST
    //--------------------------------------------------------------------------------------
    VG_INLINE core::vector<core::IResource *> & ResourceInfo::getClientsToUpdate()
    {
        return m_clientsToUpdate;
    }
    #endif

    //--------------------------------------------------------------------------------------
    VG_INLINE void ResourceInfo::setForceReimport(bool _forceReimport)
    {
        m_forceReimport = _forceReimport;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE bool ResourceInfo::needReimport() const
    {
        return m_forceReimport;
    }
}