#include "MaterialResourceList.h"
#include "engine/Resource/Material/MaterialResourceData.h"

using namespace vg::core;

namespace vg::engine
{
    VG_AUTO_REGISTER_CLASS(MaterialResourceList);

    //--------------------------------------------------------------------------------------
    MaterialResourceList::MaterialResourceList(const core::string & _name, core::IObject * _parent) :
        IResourceList(_name, _parent)
    {
        // resize of vector not supported because owner changes its address
        m_materialResources.reserve(256);
    }

    //--------------------------------------------------------------------------------------
    MaterialResourceList::~MaterialResourceList()
    {

    }

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

        _desc.registerPropertyObjectVectorEx(MaterialResourceList, m_materialResources, MaterialResource, "Material", IProperty::Flags::Resource);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MaterialResourceList::Add()
    {
        MaterialResource & matRes = m_materialResources.push_empty();

        // Update resource owners
        for (auto & matRes : m_materialResources)
            matRes.setParent(this);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MaterialResourceList::Remove()
    {
        if (m_materialResources.size() > 0)
        {
            m_materialResources.pop_back();

            // Update resource owners
            for (auto & matRes : m_materialResources)
                matRes.setParent(this);

            return true;
        }
        return false;
    }
}