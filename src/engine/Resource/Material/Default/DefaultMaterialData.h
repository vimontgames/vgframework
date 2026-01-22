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

        VG_INLINE SurfaceType       getSurfaceType              () const;
        VG_INLINE UVSource          getUVSource                 () const;
        VG_INLINE core::uint        getFrameCount               () const;

    private:
        // Alpha
        SurfaceType                 m_surfaceType;
        AlphaSource                 m_alphaSource;
        float                       m_depthFade;

        // Instance & vertex colors/alpha
        InstanceColorMask           m_instanceColorMask;
        VertexColorMask             m_vertexColorMask;

        // Culling
        CullMode                    m_cullMode;

        // UV
        UVSource                    m_UVSource;
        core::float2                m_tiling;
        core::float2                m_offset;

        // Albedo
        bool                        m_enableAlbedo;
        TextureResource             m_albedoMap;
        core::float4                m_albedoColor;

        // Normal
        bool                        m_enableNormal;
        TextureResource             m_normalMap;
        float                       m_normalStrength;

        // PBR
        bool                        m_enablePbr;
        TextureResource             m_pbrMap;       
        float                       m_occlusion;
        float                       m_roughness;
        float                       m_metalness;

        // Emissive
        bool                        m_enableEmissive;
        TextureResource             m_emissiveMap;
        core::float4                m_emissiveColor; 
        float                       m_emissiveIntensity;
    };
}

#if VG_ENABLE_INLINE
#include "DefaultMaterialData.inl"
#endif