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

        registerProperty(LightDesc, m_castShadows, "Cast Shadows");
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
        m_castShadows = _lightDesc->m_castShadows;
        setColor(_lightDesc->m_color);
        m_intensity = _lightDesc->m_intensity;
    }

    //--------------------------------------------------------------------------------------
    LightInstance::~LightInstance()
    {
        
    }

    //--------------------------------------------------------------------------------------
    bool LightInstance::Cull(const Frustum & _frustum, CullingResult * _cullingResult)
    {
        bool visible = true; // TODO

        if (visible)
        {
            _cullingResult->m_output->add(GraphicInstanceListType::All, this);
            _cullingResult->m_output->add(GraphicInstanceListType::Light, this);
            return true;
        }

        return false;
    }
}