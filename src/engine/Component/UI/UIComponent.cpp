#include "engine/Precomp.h"
#include "UIComponent.h"
#include "renderer/IPicking.h"

#if !VG_ENABLE_INLINE
#include "UIComponent.inl"
#endif

#include "UITextComponent.hpp"
#include "UIImageComponent.hpp"

using namespace vg::core;

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    bool UIComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerOptionalPropertyEx(UIComponent, m_useColor, m_color, "Color", IProperty::Flags::Color);
        registerPropertyEnum(UIComponent, gfx::HorizontalAligment, m_horizontal, "Horizontal");
        registerPropertyEnum(UIComponent, gfx::VerticalAligment, m_vertical, "Vertical");
        registerOptionalProperty(UIComponent, m_useOffset, m_offset, "Offset");

        return true;
    }

    //--------------------------------------------------------------------------------------
    UIComponent::UIComponent(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent),
        m_pickingID((renderer::PickingID)0),
        m_horizontal(gfx::HorizontalAligment::Center),
        m_vertical(gfx::VerticalAligment::Center),
        m_useOffset(false),
        m_offset(core::float3(0, 0, 0)),
        m_useColor(false),
        m_color(1, 1, 1, 1)
    {
        auto * picking = Engine::get()->GetRenderer()->GetPicking();
        m_pickingID = picking->CreatePickingID(this);
    }

    //--------------------------------------------------------------------------------------
    UIComponent::~UIComponent()
    {
        auto * picking = Engine::get()->GetRenderer()->GetPicking();
        picking->ReleasePickingID(m_pickingID);
    }
}