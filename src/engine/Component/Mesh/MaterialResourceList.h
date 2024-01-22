#pragma once

#include "engine/IResourceList.h"
#include "engine/Resource/Material/MaterialResource.h"

namespace vg::engine
{
    class MaterialResourceList : public IResourceList
    {
    public:
        VG_CLASS_DECL(MaterialResourceList, core::Object);

        MaterialResourceList(const core::string & _name, core::IObject * _parent);
        ~MaterialResourceList();

        bool Add() final override;
        bool Remove() final override;
        size_t Size() const final override;

        const core::vector<MaterialResource> & getMaterialResources() const { return m_materialResources; }

    private:
        core::vector<MaterialResource> m_materialResources;
    };
}