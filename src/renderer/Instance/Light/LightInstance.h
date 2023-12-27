#pragma once
#include "renderer/ILightInstance.h"

namespace vg::renderer
{
    class LightDesc : public ILightDesc
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(LightDesc, ILightDesc);

        LightType       GetLightType() const = 0;

    private:
        bool            m_castShadows = false;
        core::float4    m_color = core::float4(1, 1, 1, 1);
        float           m_intensity = 1.0f;
    };

    class LightInstance : public ILightInstance
    {
    public:
        VG_CLASS_DECL(LightInstance, ILightInstance);

        LightInstance(const core::string & _name, core::IObject * _parent);
        ~LightInstance();


        bool            OnUpdateRayTracing  (gfx::CommandList * _cmdList, View * _view, core::uint _index) override { return false; }
        LightType       GetLightType        () const = 0;        
    };
}

#if VG_ENABLE_INLINE
#include "LightInstance.inl"
#endif