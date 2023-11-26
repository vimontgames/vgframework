#include "SharedResource.h"

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS(SharedResource, "Shared Resource");

    //--------------------------------------------------------------------------------------
    bool SharedResource::registerProperties(vg::core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        //_desc.registerPropertyEx(SharedResource, m_path, "Path", IProperty::Flags::ReadOnly);
        _desc.registerPropertyObjectPtrVectorEx(SharedResource, m_clients, "Clients", IProperty::Flags::ReadOnly);
        //_desc.registerPropertyObjectPtrEx(SharedResource, m_object, "Shared Object", IProperty::Flags::ReadOnly | IProperty::Flags::Resource);

        return true;
    }

    //--------------------------------------------------------------------------------------
    SharedResource::SharedResource(const string & _name, IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    const core::string & SharedResource::GetResourcePath() const
    {
        return m_path;
    }

    //--------------------------------------------------------------------------------------
    const core::string SharedResource::GetResourceType() const
    {
        VG_ASSERT(m_clients.size() > 0);
        return m_clients[0]->getClassName();
    }

    //--------------------------------------------------------------------------------------
    const core::uint SharedResource::GetClientCount() const
    {
        return (uint)m_clients.size();
    }

    //--------------------------------------------------------------------------------------
    const core::IResource * SharedResource::GetClient(core::uint _index) const
    {
        return m_clients[_index];
    }
}