#pragma once
#include "renderer/ILightInstance.h"

namespace vg::renderer
{
    class LightDesc : public ILightDesc
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(LightDesc, ILightDesc);

        LightType GetLightType() const = 0;

    private:
        bool    m_castShadows = false;
        float   m_intensity = 1.0f;
    };

    class LightInstance : public ILightInstance
    {
    public:
        VG_CLASS_DECL(LightInstance, ILightInstance);

        LightInstance(const core::string & _name, core::IObject * _parent);
        ~LightInstance();

        LightType GetLightType() const = 0;        
    };
}

#if VG_ENABLE_INLINE
#include "LightInstance.inl"
#endif