#pragma once

#include "renderer/Model/Material/MaterialModel.h"

namespace vg::renderer
{
    class DefaultMaterialModel final : public MaterialModel
    {
    public:
        VG_CLASS_DECL(DefaultMaterialModel, MaterialModel);

        DefaultMaterialModel(const core::string & _name, IObject * _parent);
        ~DefaultMaterialModel();

        void                    SetSurfaceType      (SurfaceType _surfaceType) final override;
        void                    SetColor            (const core::string & _name, core::float4 _value) final override;
        void                    SetTexture          (const core::string & _name, gfx::ITexture * _value) final override;
        void                    SetFloat            (const core::string & _name, float _value) final override;
        void                    SetFloat2           (const core::string & _name, core::float2 _value) final override;

        void                    FillGPUMaterialData (GPUMaterialData * _data) const final override;

        void                    Setup               (const RenderContext & _renderContext, gfx::CommandList * _cmdList, RootConstants3D * _root3D, core::uint _index) const final override;

        SurfaceType             GetSurfaceType      () const final override { return m_surfaceType; }

    protected:
        void                    assignTexture       (gfx::Texture ** _texture, gfx::Texture * _value);

    private:
        // Alpha
        SurfaceType             m_surfaceType;
        AlphaSource             m_alphaSource;
        float                   m_depthFade;

        // Culling
        gfx::CullMode           m_cullMode;

        // UV
        UVSource                m_UVSource;
        core::float2            m_tiling;
        core::float2            m_offset;

        // Albedo
        bool                    m_enableAlbedo;
        gfx::Texture *          m_albedoMap;         
        core::float4            m_albedoColor;

        // Normal
        bool                    m_enableNormal;
        gfx::Texture *          m_normalMap;        
        float                   m_normalStrength;

        // PBR
        bool                    m_enablePbr;
        gfx::Texture *          m_pbrMap;            
        float                   m_occlusion;
        float                   m_roughness;
        float                   m_metalness;

        // Emissive
        bool                    m_enableEmissive;
        gfx::Texture *          m_emissiveMap;       
        core::float4            m_emissiveColor;
        float                   m_emissiveIntensity;
    };
}