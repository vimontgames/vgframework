#include "renderer/Precomp.h"
#include "LightInstance.h"

#if !VG_ENABLE_INLINE
#include "LightInstance.inl"
#endif

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
        registerPropertyEx(LightDesc, m_color, "Intensity", IProperty::Flags::Color);
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
    LightInstance::LightInstance(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {

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