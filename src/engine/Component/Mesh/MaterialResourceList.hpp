#include "MaterialResourceList.h"
#include "engine/Resource/Material/MaterialResourceData.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS(MaterialResourceList, "Material Resource List");

    //--------------------------------------------------------------------------------------
    bool MaterialResourceList::registerProperties(vg::core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyObjectVectorEx(MaterialResourceList, m_materialResources, MaterialResource, "Material", IProperty::Flags::Resource);

        return true;
    }

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