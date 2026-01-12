#include "engine/Precomp.h"
#include "UIComponent.h"
#include "core/IGameObject.h"
#include "renderer/IUIRenderer.h"
#include "gfx/ITexture.h"
#include "renderer/IRenderer.h"
#include "renderer/IUIManager.h"
#include "renderer/IPicking.h"
#include "engine/Engine.h"
#include "editor/Editor_Consts.h"

#if !VG_ENABLE_INLINE
#include "UIComponent.inl"
#endif

#include "Canvas/UICanvasComponent.hpp"
#include "Text/UITextComponent.hpp"
#include "Image/UIImageComponent.hpp"

using namespace vg::core;

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    bool UIComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        
        registerPropertyEnum(UIComponent, renderer::HorizontalAligment, m_horizontal, "Horizontal");
        setPropertyDescription(UIComponent, m_horizontal, "Horizontal aligment in canvas");

        registerPropertyEnum(UIComponent, renderer::VerticalAligment, m_vertical, "Vertical");
        setPropertyDescription(UIComponent, m_vertical, "Vertical aligment in canvas");

        registerPropertyEnumBitfield(UIComponent, renderer::UIItemFlags, m_UIFlags, "Flags");
        setPropertyDescription(UIComponent, m_UIFlags, "AutoResize: Resize the UI element according to its contents\nKeepAspectRatio: Respect aspect ratio when resizing element");

        registerProperty(UIComponent, m_size, "Size");
        setPropertyDescription(UIComponent, m_size, "Size of the UI element in pixels.");

        registerOptionalProperty(UIComponent, m_useCenter, m_center, "Center");
        setPropertyDescription(UIComponent, m_center, "Center of the UI element in relation to its size.");
        setPropertyRange(UIComponent, m_center, float2(0, 1));

        registerOptionalProperty(UIComponent, m_useOffset, m_offset, "Offset");
        setPropertyDescription(UIComponent, m_offset, "2D offset position of the UI element");

        registerOptionalPropertyEx(UIComponent, m_useColor, m_color, "Color", PropertyFlags::Color);
        setPropertyDescription(UIComponent, m_color, "Color of the UI element");

        return true;
    }

    //--------------------------------------------------------------------------------------
    UIComponent::UIComponent(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent),
        m_pickingID((renderer::PickingID)0),
        m_horizontal(renderer::HorizontalAligment::Center),
        m_vertical(renderer::VerticalAligment::Center),
        m_useOffset(false),
        m_offset(core::float3(0, 0, 0)),
        m_useColor(false),
        m_color(1, 1, 1, 1)
    {
        auto * renderer = Engine::get()->GetRenderer();
        m_pickingID = renderer->GetPicking()->CreatePickingID(this);
        m_uiManager = renderer->GetUIManager();
    }

    //--------------------------------------------------------------------------------------
    UIComponent::~UIComponent()
    {
        auto * picking = Engine::get()->GetRenderer()->GetPicking();
        picking->ReleasePickingID(m_pickingID);
    }

    //--------------------------------------------------------------------------------------
    void UIComponent::OnLoad()
    {
        RegisterUID();
    }

    //--------------------------------------------------------------------------------------
    core::uint2 UIComponent::GetSize() const
    {
        return getSize();
    }

    //--------------------------------------------------------------------------------------
    void UIComponent::SetSize(const core::uint2 _size)
    {
        setSize(_size);
    }

    //--------------------------------------------------------------------------------------
    core::float4 UIComponent::GetColor() const
    {
        return getColor();
    }

    //--------------------------------------------------------------------------------------
    void UIComponent::SetColor(const core::float4 & _color)
    {
        m_useColor = true;
        m_color = _color;
    }

    //--------------------------------------------------------------------------------------
    const renderer::UICanvas * UIComponent::getCanvas() const
    {
        if (auto * canvas = GetGameObject()->GetComponentInParentsT<UICanvasComponent>())
            return &canvas->getGfxCanvas();
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    renderer::UIItem UIComponent::getUIItem() const
    {
        return renderer::UIItem(m_pickingID, getMatrix(), getOffset(), m_size, getCenter(), m_horizontal, m_vertical, getColor(), m_UIFlags);
    }
}