#include "renderer/Precomp.h"
#include "Viewport.h"
#include "renderer/View/View.h"
#include "renderer/Renderer.h"
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
    }

    //--------------------------------------------------------------------------------------
    Viewport::~Viewport()
    {
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
    void Viewport::SetActive(bool _active)
    {
        auto * renderer = Renderer::get();
        for (uint i = 0; i < m_viewIDs.size(); ++i)
        {
            if (auto * view = renderer->GetView(m_viewIDs[i]))
            {
                view->SetActive(_active);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    bool Viewport::AnyActive() const
    {
        auto * renderer = Renderer::get();
        for (uint i = 0; i < m_viewIDs.size(); ++i)
        {
            if (auto * view = renderer->GetView(m_viewIDs[i]))
            {
                if (view->IsActive())
                    return true;
            }
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    void Viewport::SetVisible(bool _visible)
    {
        auto * renderer = Renderer::get();
        for (uint i = 0; i < m_viewIDs.size(); ++i)
        {
            if (auto * view = renderer->GetView(m_viewIDs[i]))
            {
                view->SetVisible(_visible);
            }
        }
    }

    //--------------------------------------------------------------------------------------
    bool Viewport::AnyVisible() const
    {
        auto * renderer = Renderer::get();
        for (uint i = 0; i < m_viewIDs.size(); ++i)
        {
            if (auto * view = renderer->GetView(m_viewIDs[i]))
            {
                if (view->IsVisible())
                    return true;
            }
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    void Viewport::AddView(gfx::ViewID _viewID)
    {
        m_viewIDs.push_back(_viewID);

        //if (auto * view = renderer->GetView(_view->GetViewID()))
        //{
        //    m_views.push_back((View *)_view);
        //    return _view->GetViewID();
        //    
        //}
        //else
        //{
        //    auto id = Renderer::get()->AddView(_view);
        //    m_views.push_back((View *)_view);
        //    return id;
        //}
    }

    //--------------------------------------------------------------------------------------
    void Viewport::RemoveView(gfx::ViewID _viewID)
    {
        m_viewIDs.remove(_viewID);

        //auto * renderer = Renderer::get();
        //if (auto * view = renderer->GetView(_viewID))
        //{
        //    m_viewIDs.remove((View *)view);
        //    //renderer->RemoveView(_viewID);
        //}
    }

    //--------------------------------------------------------------------------------------
    const core::vector<gfx::ViewID> & Viewport::GetViewIDs() const
    {
        return m_viewIDs;
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