#pragma once

#include "gfx/IViewport.h"

namespace vg::gfx
{
    class Texture;
}

namespace vg::renderer
{
    class View;

    //--------------------------------------------------------------------------------------
    // A Viewport is a collection of views rendered together
    //--------------------------------------------------------------------------------------
    class Viewport : public gfx::IViewport
    {
    public:
        const char * GetClassName() const override { return "Viewport"; }

        Viewport(const gfx::CreateViewportParams & _params);
        ~Viewport();

        void                                SetRenderTargetSize     (core::uint2 _size) final override;
        core::uint2                         GetRenderTargetSize     () const final override;

        void                                SetRenderTarget         (gfx::ITexture * _renderTarget) final override;
        gfx::ITexture *                     GetRenderTarget         () const final override;

        void                                SetFlags                (gfx::ViewportFlags _flagsToSet, gfx::ViewportFlags _flagsToRemove = (gfx::ViewportFlags)0x0) final override;
        gfx::ViewportFlags                  GetFlags                () const final override;

       void                                 SetViewportID           (gfx::ViewportID _viewportID) final override;
       gfx::ViewportID                      GetViewportID           () const final override;

       void                                 SetActive               (bool _active) final override;
       bool                                 AnyActive               () const final override;

       void                                 SetVisible              (bool _visible) final override;
       bool                                 AnyVisible              () const final override;

       void                                 AddView                 (gfx::ViewID _viewID) final override;
       void                                 RemoveView              (gfx::ViewID _viewID) final override;

       const core::vector<gfx::ViewID> &    GetViewIDs              () const final override;

       const core::string                   GetFrameGraphID         (const core::string & _name) const final override;


    private:
        gfx::ViewportID                     m_viewportID;
        gfx::ViewportFlags                  m_flags = (gfx::ViewportFlags)0x0;
        core::uint2                         m_renderTargetSize = core::uint2(0, 0);
        gfx::Texture *                      m_renderTarget = nullptr;
        core::vector<gfx::ViewID>           m_viewIDs;
    };
}