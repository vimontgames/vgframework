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

        registerPropertyEx(LightDesc, m_shadow, "Cast Shadows", PropertyFlags::NotVisible);

        registerPropertyOptionalGroupBegin(LightDesc, m_shadow, "Shadows");
        {
            registerProperty(LightDesc, m_shadowRange, "Range");
            setPropertyRange(LightDesc, m_shadowRange, float2(0.0f, 1000.0f));
            setPropertyDescription(LightDesc, m_shadowRange, "Max shadow distance");

            registerProperty(LightDesc, m_shadowBias, "Bias");
            setPropertyRange(LightDesc, m_shadowBias, float2(0.0f, 1.0f));
            setPropertyDescription(LightDesc, m_shadowBias, "Bias shadow distance");

            registerProperty(LightDesc, m_shadowSize, "Size");
            setPropertyDescription(LightDesc, m_shadowSize, "Shadow map size in world-space");

            registerProperty(LightDesc, m_shadowResolution, "Resolution");
            setPropertyDescription(LightDesc, m_shadowResolution, "Shadow map resolution in pixels");

            registerProperty(LightDesc, m_shadowIntensity, "Shadow Intensity");
            setPropertyRange(LightDesc, m_shadowIntensity, float2(0.0f, 1.0f));
            setPropertyDescription(LightDesc, m_shadowIntensity, "Amound of light occluded in shadowed areas");
        }
        registerPropertyOptionalGroupEnd(LightDesc);

        registerPropertyEx(LightDesc, m_color, "Color", PropertyFlags::Color | PropertyFlags::HDR);
        setPropertyDescription(LightDesc, m_color, "Direct Light color");

        registerProperty(LightDesc, m_intensity, "Intensity");
        setPropertyDescription(LightDesc, m_intensity, "Direct Light intensity multiplier");
        setPropertyRange(LightDesc, m_intensity, float2(0.0f, 100.0f));

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
        m_shadowBias = _lightDesc->m_shadowBias * 0.01f;
        m_shadowSize = _lightDesc->m_shadowSize;
        m_shadowResolution = _lightDesc->m_shadowResolution;
        m_shadowIntensity = m_shadow ? _lightDesc->m_shadowIntensity : 0.0f;

        setColor(_lightDesc->m_color);
        m_intensity = _lightDesc->m_intensity;
    }

    //--------------------------------------------------------------------------------------
    LightInstance::~LightInstance()
    {
        
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