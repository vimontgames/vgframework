#include "ResourceInfo.h"

namespace vg::engine
{
    //VG_REGISTER_OBJECT_CLASS(ResourceInfo, "Shared Resource");

    //--------------------------------------------------------------------------------------
    //bool ResourceInfo::registerProperties(vg::core::IClassDesc & _desc)
    //{
    //    super::registerProperties(_desc);
    //
    //    //registerPropertyEx(SharedResource, m_path, "Path", PropertyFlags::ReadOnly);
    //    registerPropertyObjectPtrVectorEx(ResourceInfo, m_clients, "Clients", PropertyFlags::ReadOnly);
    //    //registerPropertyObjectPtrEx(SharedResource, m_object, "Shared Object", PropertyFlags::ReadOnly | PropertyFlags::Resource);
    //
    //    return true;
    //}

    //--------------------------------------------------------------------------------------
    ResourceInfo::ResourceInfo(const string & _name, IObject * _parent) :
        super(_name, _parent)
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
}