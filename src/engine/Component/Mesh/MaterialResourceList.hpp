#include "MaterialResourceList.h"

using namespace vg::core;

namespace vg::engine
{
    VG_AUTO_REGISTER_CLASS(MaterialResourceList);

    //--------------------------------------------------------------------------------------
    MaterialResourceList::MaterialResourceList(const core::string & _name, core::IObject * _parent) :
        IMaterialList(_name, _parent)
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

        _desc.registerPropertyObjectVectorHelper(MaterialResourceList, m_materialResources, MaterialResource, "Material List", IProperty::Flags::Resource);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MaterialResourceList::AddMaterial()
    {
        m_materialResources.push_back({});

        // Update resource owners
        for (auto & matRes : m_materialResources)
            matRes.setParent(this);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool MaterialResourceList::RemoveMaterial()
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