#include "engine/Precomp.h"
#include "UIComponent.h"

#include "UITextComponent.hpp"
#include "UIImageComponent.hpp"

using namespace vg::core;

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    bool UIComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerOptionalProperty(UIComponent, m_useRect, m_rect, "Rect");
        registerOptionalProperty(UIComponent, m_useDepth, m_depth, "Depth");
        registerOptionalPropertyEx(UIComponent, m_useColor, m_color, "Color", IProperty::Flags::Color);

        return true;
    }

    //--------------------------------------------------------------------------------------
    UIComponent::UIComponent(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    UIComponent::~UIComponent()
    {

    }
}