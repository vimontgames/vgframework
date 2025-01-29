#pragma once
#include "renderer/ILightInstance.h"

namespace vg::renderer
{
    vg_enum_class(ShadowResolution, core::i8,
        ShadowResolution_VeryLow  = -2,
        ShadowResolution_Low      = -1,
        ShadowResolution_Medium   = 0,
        ShadowResolution_High     = 1,
        ShadowResolution_VeryHigh = 2
    );

    class LightDesc : public ILightDesc
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(LightDesc, ILightDesc);

        LightType       GetLightType() const = 0;

        bool                m_shadow                = false;
        bool                m_shadowCameraOffset    = false;   
        core::float2        m_shadowRange           = core::float2(0.1f, 100.0f);
        float               m_shadowBias            = 0.01f;
        core::uint2         m_shadowSize            = core::uint2(16, 16);
        ShadowResolution    m_shadowResolution      = ShadowResolution::ShadowResolution_Medium;
        float               m_shadowIntensity       = 1.0f;
        core::float4        m_color                 = core::float4(1, 1, 1, 1);
        float               m_intensity             = 1.0f;
    };

    class ShadowView;

    class LightInstance : public ILightInstance
    {
    public:
        VG_CLASS_DECL(LightInstance, ILightInstance);

        LightInstance(const core::string & _name, core::IObject * _parent, const LightDesc * _lightDesc = nullptr);
        ~LightInstance();

        bool                            Cull                (CullingResult * _cullingResult, View * _view) const override;
        void                            Draw                (const RenderContext & _renderContext, gfx::CommandList * _cmdList) const override;
        bool                            OnUpdateRayTracing  (gfx::CommandList * _cmdList, View * _view, core::uint _index) override { return false; }
        LightType                       GetLightType        () const = 0;    
        bool                            IsCastShadow        () const final override;
        core::uint2                     getShadowResolution () const;
        VG_INLINE float                 getIntensity        () const { return m_intensity; }

        bool                            m_shadow;
        bool                            m_shadowCameraOffset = false;
        core::float2                    m_shadowRange;
        float                           m_shadowBias;
        core::uint2                     m_shadowSize;
        ShadowResolution                m_shadowResolution;
        float                           m_shadowIntensity;
        float                           m_intensity;
    };
}

#if VG_ENABLE_INLINE
#include "LightInstance.inl"
#endif