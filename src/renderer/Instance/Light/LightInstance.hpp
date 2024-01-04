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
            //setPropertyRange(LightDesc, m_shadowRange, float2(0.0f, 100.0f));

            registerProperty(LightDesc, m_shadowBias, "Bias");
            setPropertyRange(LightDesc, m_shadowBias, float2(0.0001f, 0.01f));

            registerProperty(LightDesc, m_shadowSize, "Size");
            registerProperty(LightDesc, m_shadowResolution, "Resolution");

            registerProperty(LightDesc, m_shadowIntensity, "Intensity");
            setPropertyRange(LightDesc, m_shadowIntensity, float2(0.0f, 1.0f));
        }
        registerPropertyOptionalGroupEnd(LightDesc);


        registerPropertyEx(LightDesc, m_color, "Color", IProperty::Flags::Color | IProperty::Flags::HDR);
        registerProperty(LightDesc, m_intensity, "Intensity");

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