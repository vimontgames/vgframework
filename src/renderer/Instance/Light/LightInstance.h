#pragma once
#include "renderer/ILightInstance.h"

namespace vg::renderer
{
    vg_enum_class(vg::renderer, ShadowResolution, core::i8,
        VeryLow  = -2,
        Low      = -1,
        Medium   = 0,
        High     = 1,
        VeryHigh = 2
    );

    vg_enum_class(vg::renderer, ShadowCameraMode, core::u8,
        Fixed = 0,
        FollowCameraTranslation = 1,
        FitCameraFrustum = 2
    );

    class LightDesc : public ILightDesc
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(LightDesc, ILightDesc);

        LightType       GetLightType() const = 0;

        bool                m_shadow            = false;
        ShadowCameraMode    m_shadowCameraMode  = ShadowCameraMode::Fixed;
        core::float2        m_shadowRange       = core::float2(0.1f, 100.0f);
        float               m_shadowBias        = 0.01f;
        core::uint2         m_shadowSize        = core::uint2(16, 16);
        ShadowResolution    m_shadowResolution  = ShadowResolution::Medium;
        float               m_shadowIntensity   = 1.0f;
        core::float4        m_color             = core::float4(1, 1, 1, 1);
        float               m_intensity         = 1.0f;
    };

    class ShadowView;

    class LightInstance : public ILightInstance
    {
    public:
        VG_CLASS_DECL(LightInstance, ILightInstance);

        LightInstance(const core::string & _name, core::IObject * _parent, const LightDesc * _lightDesc = nullptr);
        ~LightInstance();

        GraphicInstanceType         GetGraphicIntanceType   () const final override { return GraphicInstanceType::Light; }

        bool                        Cull                    (const ViewCullingOptions & _cullingOptions, CullingResult * _cullingResult) override;
        void                        Draw                    (const RenderContext & _renderContext, gfx::CommandList * _cmdList) const override;

        LightType                   GetLightType            () const = 0;    
        bool                        IsCastShadow            () const final override;
        core::uint2                 getShadowResolution     () const;
        VG_INLINE float             getIntensity            () const { return m_intensity; }

        bool                        m_shadow;
        ShadowCameraMode            m_shadowCameraMode;
        core::float2                m_shadowRange;
        float                       m_shadowBias;
        core::uint2                 m_shadowSize;
        ShadowResolution            m_shadowResolution;
        float                       m_shadowIntensity;
        float                       m_intensity;
    };
}

#if VG_ENABLE_INLINE
#include "LightInstance.inl"
#endif