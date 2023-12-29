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

        void                        OnPropertyChanged       (IObject * _object, const IProperty & _prop, bool _notifyParent) override;

    private:
        TextureResource             m_albedoMap;
        core::float4                m_albedoColor = core::float4(1, 1, 1, 1);

        TextureResource             m_normalMap;
        float                       m_normalStrength = 1.0f;

        TextureResource             m_pbr;       
        float                       m_occlusion     = 1.0f;
        float                       m_roughness     = 1.0f;
        float                       m_metalness     = 1.0f;
    };
}