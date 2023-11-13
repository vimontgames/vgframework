#pragma once

#include "engine/IMaterialList.h"
#include "engine/Resource/Material/MaterialResource.h"

namespace vg::engine
{
    class MaterialResourceList : public IMaterialList
    {
    public:
        VG_CLASS_DECL(MaterialResourceList, core::Object);

        MaterialResourceList(const core::string & _name, core::IObject * _parent);
        ~MaterialResourceList();

        bool AddMaterial() final override;
        bool RemoveMaterial() final override;

    private:
        core::vector<MaterialResource> m_materialResources;
    };
}