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

        void SetSurfaceType(gfx::SurfaceType _surfaceType) final override;
        void SetColor(const core::string & _name, core::float4 _value) final override;
        void SetTexture(const core::string & _name, gfx::ITexture * _value) final override;
        void SetFloat(const core::string & _name, float _value) final override;

        void Setup(const RenderContext & _renderContext, gfx::CommandList * _cmdList, RootConstants3D * _root3D, core::uint _index) const final override;

    protected:
        void assignTexture(gfx::Texture ** _texture, gfx::Texture * _value);

    private:
        gfx::Texture *  m_albedoMap         = nullptr;
        core::float4    m_albedoColor       = core::float4(1,1,1,1);

        gfx::Texture *  m_normaMap          = nullptr;
        float           m_normalStrength    = 1.0f;

        gfx::Texture *  m_pbrMap            = nullptr;
        float           m_occlusion         = 1.0f;
        float           m_roughness         = 1.0f;
        float           m_metalness         = 1.0f;
    };
}