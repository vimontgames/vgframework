#include "renderer/Precomp.h"
#include "LightInstance.h"

#if !VG_ENABLE_INLINE
#include "LightInstance.inl"
#endif

#include "Directional/DirectionalLightInstance.hpp"
#include "Omni/OmniLightInstance.hpp"

using namespace vg::core;
using namespace vg::gfx;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    // LightDesc
    //--------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------
    bool LightDesc::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(LightDesc, m_shadow, "Cast Shadows");

        registerPropertyOptionalGroupBegin(LightDesc, m_shadow, "Shadow Settings");
        {
            registerProperty(LightDesc, m_shadowRange, "Range");
            setPropertyRange(LightDesc, m_shadowRange, float2(0.0f, 256.0f));

            registerProperty(LightDesc, m_shadowBias, "Bias");
            setPropertyRange(LightDesc, m_shadowBias, float2(0.0001f, 0.01f));

            registerProperty(LightDesc, m_shadowSize, "Size");
            registerProperty(LightDesc, m_shadowResolution, "Resolution");

            registerProperty(LightDesc, m_shadowIntensity, "Shadow Intensity");
            setPropertyRange(LightDesc, m_shadowIntensity, float2(0.0f, 1.0f));
        }
        registerPropertyOptionalGroupEnd(LightDesc);

        registerPropertyEx(LightDesc, m_color, "Color", PropertyFlags::Color | PropertyFlags::HDR);
        setPropertyDescription(LightDesc, m_color, "Direct Light color");

        registerProperty(LightDesc, m_intensity, "Intensity");
        setPropertyDescription(LightDesc, m_intensity, "Direct Light intensity multiplier");

        registerPropertyEx(LightDesc, m_ambient, "Ambient", PropertyFlags::Color | PropertyFlags::HDR);
        setPropertyDescription(LightDesc, m_ambient, "Ambient Light color");

        return true;
    }

    //--------------------------------------------------------------------------------------
    // LightInstance
    //--------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------
    bool LightInstance::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);



        return true;
    }

    //--------------------------------------------------------------------------------------
    LightInstance::LightInstance(const core::string & _name, core::IObject * _parent, const LightDesc * _lightDesc) :
        super(_name, _parent)
    {
        m_shadow = _lightDesc->m_shadow;
        m_shadowRange = _lightDesc->m_shadowRange;
        m_shadowBias = _lightDesc->m_shadowBias;
        m_shadowSize = _lightDesc->m_shadowSize;
        m_shadowResolution = _lightDesc->m_shadowResolution;
        m_shadowIntensity = m_shadow ? _lightDesc->m_shadowIntensity : 0.0f;

        setColor(_lightDesc->m_color);
        SetAmbient(_lightDesc->m_ambient);
        m_intensity = _lightDesc->m_intensity;
    }

    //--------------------------------------------------------------------------------------
    LightInstance::~LightInstance()
    {
        
    }

    //--------------------------------------------------------------------------------------
    void LightInstance::SetAmbient(const float3 & _ambient)
    {
        setAmbient(_ambient);
    }

    //--------------------------------------------------------------------------------------
    const core::float3 & LightInstance::GetAmbient() const
    {
        return getAmbient();
    }

    //--------------------------------------------------------------------------------------
    bool LightInstance::Cull(CullingResult * _cullingResult, View * _view)
    {
        bool visible = true; // TODO

        if (visible)
        {
            _cullingResult->m_output->add(GraphicInstanceListType::All, this);
            _cullingResult->m_output->add(GetLightType(), this);

            return true;
        }

        return false;
    }
}