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
    const core::string & ResourceInfo::GetResourcePath() const
    {
        return m_path;
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
    }

    //--------------------------------------------------------------------------------------
    bool ResourceInfo::removeClient(core::IResource * _clientToRemove)
    {
        return vector_helper::remove(m_clients, _clientToRemove);
    }

    //--------------------------------------------------------------------------------------
    bool ResourceInfo::replaceClient(core::IResource * _clientToReplace, core::IResource * _newClient)
    {
        auto it = std::find(m_clients.begin(), m_clients.end(), _clientToReplace);
        if (it != m_clients.end())
        {
            VG_DEBUGPRINT("[Resource] Resource A 0x%016llx was a client of Resource \"%s\"\n", _clientToReplace, m_path.c_str());
            *it = _newClient;
            return true;
        }
        return false;
    }
}