#include "UICanvasComponent.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(UICanvasComponent, "UICanvas", "UI", "UI Canvas", editor::style::icon::Canvas, 2);

    //--------------------------------------------------------------------------------------
    bool UICanvasComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEnumEx(UICanvasComponent, gfx::ViewportTarget, m_canvas.m_viewportTarget, "Target", PropertyFlags::ReadOnly);
        setPropertyDescription(UICanvasComponent, m_canvas.m_viewportTarget, "Target viewport type");

        registerPropertyEx(UICanvasComponent, m_canvas.m_viewportIndex, "Viewport", PropertyFlags::ReadOnly);
        setPropertyDescription(UICanvasComponent, m_canvas.m_viewportIndex, "Target viewport index");

        registerOptionalPropertyEnumBitfield(UICanvasComponent, m_canvas.m_useViewMask, renderer::ViewMask, m_canvas.m_viewMask, "Views");
        setPropertyDescription(UICanvasComponent, m_canvas.m_viewMask, "Select the views to render UI to. Disable to render fullscreen.");

        registerProperty(UICanvasComponent, m_canvas.m_resolution, "Resolution");
        setPropertyDescription(UICanvasComponent, m_canvas.m_resolution, "Reference resolution of UI elements");

        registerPropertyEnum(UICanvasComponent, renderer::CanvasType, m_canvas.m_canvasType, "Type");
        setPropertyDescription(UICanvasComponent, m_canvas.m_canvasType, "Use either 2D screen positions or 3D world positions for the canvas and its contents");

        return true;
    }

    //--------------------------------------------------------------------------------------
    UICanvasComponent::UICanvasComponent(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    UICanvasComponent::~UICanvasComponent()
    {

    }

    //--------------------------------------------------------------------------------------
    void UICanvasComponent::SetViewIndex(core::uint _viewIndex)
    {
        m_canvas.m_viewMask = (renderer::ViewMask)(1<<_viewIndex);
    }

    //--------------------------------------------------------------------------------------
    void UICanvasComponent::Update(const Context & _context)
    {
        m_canvas.m_pickingID = m_pickingID;
        m_canvas.m_matrix = getMatrix();
        m_canvas.m_offset = m_offset;
        m_canvas.m_size = m_size;
        m_canvas.m_alignX = m_horizontal;
        m_canvas.m_alignY = m_vertical;
        m_canvas.m_color = m_color;
        m_canvas.m_flags = m_UIFlags;

        m_canvas.m_resolution = m_canvas.m_resolution;

        getUIManager()->AddCanvas(&m_canvas, getUIItem(), _context.m_world);
    }
}