#pragma once
#include "renderer/ILightInstance.h"

namespace vg::renderer
{
    class LightDesc : public ILightDesc
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(LightDesc, ILightDesc);

        LightType       GetLightType() const = 0;

        bool            m_shadow            = false;
        core::float2    m_shadowRange       = core::float2(0.1f, 100.0f);
        float           m_shadowBias        = 0.001f;
        core::uint2     m_shadowSize        = core::uint2(16, 16);
        core::uint2     m_shadowResolution  = core::uint2(1024, 1024);
        float           m_shadowIntensity   = 1.0f;
        core::float4    m_color             = core::float4(1, 1, 1, 1);
        core::float3    m_ambient           = core::float3(0,0,0);
        float           m_intensity         = 1.0f;
    };

    class ShadowView;

    class LightInstance : public ILightInstance
    {
    public:
        VG_CLASS_DECL(LightInstance, ILightInstance);

        LightInstance(const core::string & _name, core::IObject * _parent, const LightDesc * _lightDesc = nullptr);
        ~LightInstance();

        bool                            Cull                (CullingResult * _cullingResult, View * _view) override;
        bool                            OnUpdateRayTracing  (gfx::CommandList * _cmdList, View * _view, core::uint _index) override { return false; }
        LightType                       GetLightType        () const = 0;    
        bool                            IsCastShadow        () const { return m_shadow;}
        void                            SetAmbient          (const float3 & _ambient) final override;
        const core::float3 &            GetAmbient          () const final override;

        VG_INLINE float                 getIntensity        () const { return m_intensity; }
        VG_INLINE void                  setAmbient          (const float3 & _ambient);
        VG_INLINE const core::float3 &  getAmbient          () const;

        core::float3                    m_ambient = core::float3(0, 0, 0);
        bool                            m_shadow;
        core::float2                    m_shadowRange;
        float                           m_shadowBias;
        core::uint2                     m_shadowSize;
        core::uint2                     m_shadowResolution;
        float                           m_shadowIntensity;
        float                           m_intensity;
    };
}

#if VG_ENABLE_INLINE
#include "LightInstance.inl"
#endif