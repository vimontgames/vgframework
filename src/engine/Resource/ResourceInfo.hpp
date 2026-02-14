#include "ResourceInfo.h"

#if !VG_ENABLE_INLINE
#include "ResourceInfo.inl"
#endif

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    ResourceInfo::ResourceInfo(const string & _path) :
        super(io::getFileName(_path)),
        m_path(_path)
    {

    }

    //--------------------------------------------------------------------------------------
    const core::string & ResourceInfo::GetPath() const
    {
        return m_path;
    }

    //--------------------------------------------------------------------------------------
    const core::string ResourceInfo::GetFolder() const
    {
        return io::RemoveStartFolder(io::getFileFolder(m_path), "data");
    }

    //--------------------------------------------------------------------------------------
    const core::string ResourceInfo::GetFilename() const
    {
        return io::getFileNameWithoutExt(m_path);
    }

    //--------------------------------------------------------------------------------------
    const core::string ResourceInfo::GetExtension() const
    {
        return io::getFileExtension(m_path);
    }

    //--------------------------------------------------------------------------------------
    const core::u64 ResourceInfo::GetRawFileSize() const
    {
        return m_rawFileSize;
    }

    //--------------------------------------------------------------------------------------
    const core::u64 ResourceInfo::GetCookedFileSize() const
    {
        return m_cookedFileSize;
    }

    //--------------------------------------------------------------------------------------
    const io::FileAccessTime ResourceInfo::GetLastCookDate() const
    {
        return m_lastCookDate;
    }

    //--------------------------------------------------------------------------------------
    const io::FileAccessTime ResourceInfo::GetLastModifiedDate() const
    {
        return m_lastModifiedDate;
    }

    //--------------------------------------------------------------------------------------
    const float ResourceInfo::GetCookingDuration() const
    {
        return m_cookingDuration;
    }

    //--------------------------------------------------------------------------------------
    const float ResourceInfo::GetLoadingDuration() const
    {
        return m_loadingDuration;
    }

    //--------------------------------------------------------------------------------------
    const core::string ResourceInfo::GetResourceType() const
    {
        VG_ASSERT(m_clients.size() > 0);
        return m_clients[0]->GetClassName();
    }

    //--------------------------------------------------------------------------------------
    const core::uint ResourceInfo::GetClientCount() const
    {
        return (uint)m_clients.size();
    }

    //--------------------------------------------------------------------------------------
    const core::IResource * ResourceInfo::GetClient(core::uint _index) const
    {
        return m_clients[_index];
    }

    //--------------------------------------------------------------------------------------
    void ResourceInfo::addClient(core::IResource * _client)
    {
        VG_ASSERT(nullptr != _client);
        VG_ASSERT(!m_path.empty());
        m_clients.push_back(_client);

        #if USE_CLIENTS_TO_UPDATE_LIST
        m_clientsToUpdate.push_back(_client);
        #endif
    }

    //--------------------------------------------------------------------------------------
    bool ResourceInfo::removeClient(core::IResource * _clientToRemove)
    {
        #if USE_CLIENTS_TO_UPDATE_LIST
        vector_helper::remove(m_clientsToUpdate, _clientToRemove);
        #endif
        return vector_helper::remove(m_clients, _clientToRemove);
    }

    //--------------------------------------------------------------------------------------
    bool ResourceInfo::replaceClient(core::IResource * _clientToReplace, core::IResource * _newClient)
    {
        #if USE_CLIENTS_TO_UPDATE_LIST
        auto it2 = std::find(m_clientsToUpdate.begin(), m_clientsToUpdate.end(), _clientToReplace);
        if (it2 != m_clientsToUpdate.end())
            *it2 = _newClient;
        #endif

        auto it = std::find(m_clients.begin(), m_clients.end(), _clientToReplace);
        if (it != m_clients.end())
        {
            VG_DEBUGPRINT("[Resource] Resource A 0x%016llx was a client of Resource \"%s\"\n", _clientToReplace, m_path.c_str());
            *it = _newClient;
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    void ResourceInfo::setCookingTime(float _cookingTime)
    {
        m_cookingDuration = _cookingTime;
    }

    //--------------------------------------------------------------------------------------
    void ResourceInfo::setLoadingTime(float _loadingTime)
    {
        m_loadingDuration = _loadingTime;
    }

    //--------------------------------------------------------------------------------------
    void ResourceInfo::setRawFileSize(core::u64 _fileSize)
    {
        m_rawFileSize = _fileSize;
    }

    //--------------------------------------------------------------------------------------
    void ResourceInfo::setCookedFileSize(core::u64 _fileSize)
    {
        m_cookedFileSize = _fileSize;
    }

    //--------------------------------------------------------------------------------------
    void ResourceInfo::setLastModifiedDate(core::io::FileAccessTime _lastModified)
    {
        m_lastModifiedDate = _lastModified;
    }

    //--------------------------------------------------------------------------------------
    void ResourceInfo::setLastCookDate(core::io::FileAccessTime _lastCooked)
    {
        m_lastCookDate = _lastCooked;
    }
}