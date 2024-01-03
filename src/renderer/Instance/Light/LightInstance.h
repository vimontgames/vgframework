#pragma once
#include "renderer/ILightInstance.h"

namespace vg::renderer
{
    class LightDesc : public ILightDesc
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(LightDesc, ILightDesc);

        LightType       GetLightType() const = 0;

        ShadowType      m_shadowType        = ShadowType::None;
        core::float2    m_shadowRange       = core::float2(0.1f, 100.0f);
        float           m_shadowBias        = 0.001f;
        core::uint2     m_shadowSize        = core::uint2(16, 16);
        core::uint2     m_shadowResolution  = core::uint2(1024, 1024);
        core::float4    m_color             = core::float4(1, 1, 1, 1);
        float           m_intensity         = 1.0f;
    };

    class ShadowView;

    class LightInstance : public ILightInstance
    {
    public:
        VG_CLASS_DECL(LightInstance, ILightInstance);

        LightInstance(const core::string & _name, core::IObject * _parent, const LightDesc * _lightDesc = nullptr);
        ~LightInstance();

        bool            Cull                (CullingResult * _cullingResult, View * _view) override;
        bool            OnUpdateRayTracing  (gfx::CommandList * _cmdList, View * _view, core::uint _index) override { return false; }
        LightType       GetLightType        () const = 0;    
        ShadowType      GetShadowType       () const { return m_shadowType;}

        VG_INLINE float getIntensity() const { return m_intensity; }

        ShadowType      m_shadowType;
        core::float2    m_shadowRange;
        float           m_shadowBias;
        core::uint2     m_shadowSize;
        core::uint2     m_shadowResolution;
        float           m_intensity;
    };
}

#if VG_ENABLE_INLINE
#include "LightInstance.inl"
#endif