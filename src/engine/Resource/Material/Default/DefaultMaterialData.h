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
        // Common
        UVSource                    m_UVSource          = UVSource::UV0;
        core::float2                m_tiling = core::float2(1, 1);
        core::float2                m_offset = core::float2(0, 0);

        // Albedo
        bool                        m_enableAlbedo      = true;
        TextureResource             m_albedoMap;
        core::float4                m_albedoColor       = core::float4(1, 1, 1, 1);

        // Normal
        bool                        m_enableNormal      = true;
        TextureResource             m_normalMap;
        float                       m_normalStrength    = 1.0f;

        // PBR
        bool                        m_enablePbr         = true;
        TextureResource             m_pbrMap;       
        float                       m_occlusion         = 1.0f;
        float                       m_roughness         = 1.0f;
        float                       m_metalness         = 1.0f;

        // Emissive
        bool                        m_enableEmissive    = true;
        TextureResource             m_emissiveMap;
        core::float4                m_emissiveColor     = core::float4(1,1,1,1); 
        float                       m_emissiveIntensity = 1.0f;
    };
}