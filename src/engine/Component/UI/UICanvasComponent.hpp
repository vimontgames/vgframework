#include "UICanvasComponent.h"
#include "editor/Editor_Consts.h"
#include "core/IGameObject.h"
#include "gfx/IViewGUI.h"
#include "renderer/IRenderer.h"
#include "engine/Engine.h"
#include "engine/EngineOptions.h"

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

        registerOptionalProperty(UICanvasComponent, m_canvas.m_useViewIndex, m_canvas.m_viewIndex, "View");
        setPropertyDescription(UICanvasComponent, m_canvas.m_viewIndex, "Render UI to a specific view. Full viewport is used if not specified");

        registerProperty(UICanvasComponent, m_canvas.m_resolution, "Resolution");
        setPropertyDescription(UICanvasComponent, m_canvas.m_resolution, "Reference resolution of UI elements");

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
        m_canvas.m_viewIndex = _viewIndex;
    }

    //--------------------------------------------------------------------------------------
    void UICanvasComponent::Update(const Context & _context)
    {
        m_canvas.m_pickingID = m_pickingID;
        m_canvas.m_matrix = _context.m_gameObject->GetGlobalMatrix();
        m_canvas.m_size = m_size;
        m_canvas.m_alignX = m_horizontal;
        m_canvas.m_alignY = m_vertical;
        m_canvas.m_color = m_color;
        m_canvas.m_flags = m_UIFlags;

        m_canvas.m_resolution = m_canvas.m_resolution;

        if (auto * gui = getGUI(_context.m_world))
        {
            auto desc = gfx::UIItem(m_pickingID, getMatrix(), m_size, m_horizontal, m_vertical, getColor(), m_UIFlags);
            gui->AddCanvas(&m_canvas, desc);
        }
    }
}