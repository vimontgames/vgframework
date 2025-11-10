#pragma once

#include "engine/Resource/ResourceList.h"
#include "engine/Resource/Material/MaterialResource.h"

namespace vg::engine
{
    class MaterialResourceList : public ResourceList<MaterialResource>
    {
    public:
        VG_CLASS_DECL(MaterialResourceList, ResourceList<MaterialResource>);

        MaterialResourceList(const core::string & _name, core::IObject * _parent);
        ~MaterialResourceList();
    };
}