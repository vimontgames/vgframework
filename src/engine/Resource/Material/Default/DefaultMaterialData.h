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

        bool                        RegisterUID                 () override;
        renderer::IMaterialModel *  CreateRendererMaterialModel () const final override;

    private:
        UVSource                    m_UVSource          = UVSource::UV0;
        TextureResource             m_albedoMap;
        core::float4                m_albedoColor       = core::float4(1, 1, 1, 1);

        TextureResource             m_normalMap;
        float                       m_normalStrength    = 1.0f;

        TextureResource             m_pbrMap;       
        float                       m_occlusion         = 1.0f;
        float                       m_roughness         = 1.0f;
        float                       m_metalness         = 1.0f;
        core::float2                m_tiling            = core::float2(1, 1);
    };
}