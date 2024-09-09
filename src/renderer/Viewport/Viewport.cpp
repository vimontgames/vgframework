#include "renderer/Precomp.h"
#include "Viewport.h"
#include "renderer/View/View.h"
#include "renderer/Renderer.h"
#include "renderer/View/ViewGUI.h"
#include "gfx/FrameGraph/FrameGraph.h"

using namespace vg::core;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    Viewport::Viewport(const gfx::CreateViewportParams & _params) :
        m_renderTargetSize(_params.size)
    {
        m_viewportID.target = _params.target;

        if (_params.dest)
        {
            VG_SAFE_INCREASE_REFCOUNT(_params.dest);
            m_renderTarget = (gfx::Texture *)_params.dest;
        }

        m_viewportGUI = new ViewGUI(this, nullptr);
    }

    //--------------------------------------------------------------------------------------
    Viewport::~Viewport()
    {
        VG_SAFE_DELETE(m_viewportGUI);
        m_viewIDs.clear();
        VG_SAFE_RELEASE(m_renderTarget);
    }

    //--------------------------------------------------------------------------------------
    void Viewport::SetViewportID(gfx::ViewportID _viewportID)
    {
        m_viewportID = _viewportID;
    }

    //--------------------------------------------------------------------------------------
    gfx::ViewportID Viewport::GetViewportID() const
    {
        return m_viewportID;
    }

    //--------------------------------------------------------------------------------------
    void Viewport::SetFlags(gfx::ViewportFlags _flagsToSet, gfx::ViewportFlags _flagsToRemove)
    {
        m_flags = (gfx::ViewportFlags)((std::underlying_type<gfx::ViewportFlags>::type(m_flags) & ~std::underlying_type<gfx::ViewportFlags>::type(_flagsToRemove)) | (std::underlying_type<gfx::ViewportFlags>::type(_flagsToSet)));
    }

    //--------------------------------------------------------------------------------------
    gfx::ViewportFlags Viewport::GetFlags() const
    {
        return m_flags;
    }

    //--------------------------------------------------------------------------------------
    void Viewport::SetRenderTargetSize(uint2 _size)
    {
        m_renderTargetSize = _size;

        auto * renderer = Renderer::get();
        for (uint i = 0; i < m_viewIDs.size(); ++i)
        {
            if (auto * view = renderer->GetView(m_viewIDs[i]))
                view->SetRenderTargetSize(_size);
        }
    }

    //--------------------------------------------------------------------------------------
    uint2 Viewport::GetRenderTargetSize() const
    {
        return m_renderTargetSize;
    }

    //--------------------------------------------------------------------------------------
    void Viewport::SetFocused(bool _active)
    {
        auto * renderer = Renderer::get();

        for (const auto & pair : m_viewIDs)
        {
            if (auto * view = renderer->GetView(pair.second))
                view->SetFocused(_active);
        }
    }

    //--------------------------------------------------------------------------------------
    bool Viewport::AnyFocused() const
    {
        auto * renderer = Renderer::get();

        for (const auto & pair : m_viewIDs)
        {
            if (auto * view = renderer->GetView(pair.second))
            {
                if (view->IsFocused())
                    return true;
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    void Viewport::SetVisible(bool _visible)
    {
        auto * renderer = Renderer::get();

        for (const auto & pair : m_viewIDs)
        {
            if (auto * view = renderer->GetView(pair.second))
                view->SetVisible(_visible);
        }
    }

    //--------------------------------------------------------------------------------------
    bool Viewport::AnyVisible() const
    {
        auto * renderer = Renderer::get();

        for (const auto & pair : m_viewIDs)
        {
            if (auto * view = renderer->GetView(pair.second))
            {
                if (view->IsVisible())
                    return true;
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    void Viewport::SetRender(bool _render)
    {
        auto * renderer = Renderer::get();

        for (const auto & pair : m_viewIDs)
        {
            if (auto * view = renderer->GetView(pair.second))
                view->SetRender(_render);
        }
    }

    //--------------------------------------------------------------------------------------
    bool Viewport::AnyRender() const
    {
        auto * renderer = Renderer::get();

        for (const auto & pair : m_viewIDs)
        {
            if (auto * view = renderer->GetView(pair.second))
            {
                if (/*view->IsVisible() &&*/ view->IsRender())
                    return true;
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    void Viewport::AddView(core::u8 _index, gfx::ViewID _viewID)
    {
        m_viewIDs.insert({ _index, _viewID });
    }

    //--------------------------------------------------------------------------------------
    void Viewport::RemoveView(core::u8 _index, gfx::ViewID _viewID)
    {
        m_viewIDs.erase(_index);
    }

    //--------------------------------------------------------------------------------------
    const core::map<gfx::ViewIndex, gfx::ViewID> & Viewport::GetViewIDs() const
    {
        return m_viewIDs;
    }

    //--------------------------------------------------------------------------------------
    gfx::IViewGUI * Viewport::GetViewportGUI() const
    {
        return VG_SAFE_STATIC_CAST(gfx::IViewGUI, m_viewportGUI);
    }

    //--------------------------------------------------------------------------------------
    const core::string Viewport::GetFrameGraphID(const core::string & _name) const
    {
        return gfx::RenderPassContext::MakeFrameGraphID(_name, m_viewportID);
    }

    //--------------------------------------------------------------------------------------
    void Viewport::SetRenderTarget(gfx::ITexture * _renderTarget)
    {
        if (_renderTarget != m_renderTarget)
        {
            VG_SAFE_RELEASE(m_renderTarget);
            m_renderTarget = (gfx::Texture*)_renderTarget;
            VG_SAFE_INCREASE_REFCOUNT(m_renderTarget);

            auto * renderer = Renderer::get();

            for (uint i = 0; i < m_viewIDs.size(); ++i)
            {
                if (auto * view = renderer->GetView(m_viewIDs[i]))
                    view->SetRenderTarget(_renderTarget);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    gfx::ITexture * Viewport::GetRenderTarget() const
    {
        return m_renderTarget;
    }
}