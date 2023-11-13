#include "MaterialResourceList.h"

using namespace vg::core;

namespace vg::engine
{
    VG_AUTO_REGISTER_CLASS(MaterialResourceList);

    //--------------------------------------------------------------------------------------
    bool MaterialResourceList::registerClass(vg::core::IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(MaterialResourceList, "Material Resource List", IClassDesc::Flags::None))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MaterialResourceList::registerProperties(vg::core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerPropertyObjectVectorHelper(MaterialResourceList, m_materialResources, MaterialResource, "Materials", IProperty::Flags::None);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MaterialResourceList::AddMaterial()
    {
        m_materialResources.push_back({});
        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MaterialResourceList::RemoveMaterial()
    {
        if (m_materialResources.size() > 0)
        {
            m_materialResources.pop_back();
            return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    MaterialResourceList::MaterialResourceList(const core::string & _name, core::IObject * _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    MaterialResourceList::~MaterialResourceList()
    {

    }
}