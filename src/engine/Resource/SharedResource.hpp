#include "SharedResource.h"

namespace vg::engine
{
    VG_AUTO_REGISTER_CLASS(SharedResource);

    //--------------------------------------------------------------------------------------
    SharedResource::SharedResource(const string & _name, IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    bool SharedResource::registerProperties(vg::core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        //_desc.registerPropertyHelperEx(SharedResource, m_path, "Path", IProperty::Flags::ReadOnly);
        _desc.registerPropertyObjectRefVectorHelper(SharedResource, m_clients, "Clients", IProperty::Flags::ReadOnly);
        //_desc.registerPropertyObjectRefHelper(SharedResource, m_object, "Shared Object", IProperty::Flags::ReadOnly | IProperty::Flags::Resource);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool SharedResource::registerClass(vg::core::IFactory & _factory)
    {
        if (IClassDesc * desc = _factory.registerClassHelper(SharedResource, "Shared Resource", IClassDesc::Flags::None))
            registerProperties(*desc);

        return true;
    }
}