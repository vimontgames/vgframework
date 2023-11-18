#pragma once

#include "engine/Resource/Material/MaterialData.h"

namespace vg::engine
{
    class DefaultMaterialData : public MaterialData
    {
    public:
        VG_CLASS_DECL(DefaultMaterialData, MaterialData);

        DefaultMaterialData(const core::string & _name = "", IObject * _parent = nullptr);
        ~DefaultMaterialData();

        renderer::IMaterialModel *  CreateRendererMaterialModel() const final override;

        void                        onResourceLoaded        (core::IResource * _resource);
        void                        onResourceUnloaded      (core::IResource * _resource);

        void                        onPropertyChanged       (IObject * _object, const IProperty & _prop) override;

    private:
        TextureResource m_albedoMap;
        core::float4    m_albedoColor = core::float4(1, 1, 1, 1);
        TextureResource m_normalMap;
    };
}