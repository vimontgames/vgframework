#pragma once
#include "renderer/ILightInstance.h"

namespace vg::renderer
{
    class LightDesc : public ILightDesc
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(LightDesc, ILightDesc);

        LightType       GetLightType() const = 0;

        core::float4    m_color = core::float4(1, 1, 1, 1);
        float           m_intensity = 1.0f;
        bool            m_castShadows = false;

    };

    class LightInstance : public ILightInstance
    {
    public:
        VG_CLASS_DECL(LightInstance, ILightInstance);

        LightInstance(const core::string & _name, core::IObject * _parent, const LightDesc * _lightDesc = nullptr);
        ~LightInstance();

        bool            Cull                (const Frustum & _frustum, CullingResult * _cullingResult) override;
        bool            OnUpdateRayTracing  (gfx::CommandList * _cmdList, View * _view, core::uint _index) override { return false; }
        LightType       GetLightType        () const = 0;    

        VG_INLINE bool  isCastShadow() const { return m_castShadows; }
        VG_INLINE float getIntensity() const { return m_intensity; }

        float           m_intensity = 1.0f;
        bool            m_castShadows = false;
    };
}

#if VG_ENABLE_INLINE
#include "LightInstance.inl"
#endif