#include "engine/Precomp.h"
#include "SnapComponent.h"
#include "editor/Editor_Consts.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(SnapComponent, "Snap", "Editor", "Override Snap Settings", editor::style::icon::Snap, 0);

    //--------------------------------------------------------------------------------------
    bool SnapComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerOptionalProperty(SnapComponent, m_snapTranslation, m_translation, "Translate");
        registerOptionalProperty(SnapComponent, m_snapRotation, m_rotation, "Rotate");
        registerOptionalProperty(SnapComponent, m_snapScale, m_scale, "Scale");

        return true;
    }

    //--------------------------------------------------------------------------------------
    SnapComponent::SnapComponent(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {
        EnableUpdateFlags(UpdateFlags::Update, false);
    }

    //--------------------------------------------------------------------------------------
    SnapComponent::~SnapComponent()
    {

    }

    //--------------------------------------------------------------------------------------
    bool SnapComponent::TryGetSnapTranslate(core::float3 & _translate) const
    {
        if (m_snapTranslation)
        {
            _translate = m_translation;
            return true;
        }

        return false;
    }
    //--------------------------------------------------------------------------------------
    bool SnapComponent::TryGetSnapRotate(float & _rotate) const
    {
        if (m_snapRotation)
        {
            _rotate = m_rotation;
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool SnapComponent::TryGetSnapScale(float & _scale) const 
    {
        if (m_snapScale)
        {
            _scale = m_scale;
            return true;
        }

        return false;
    }
}