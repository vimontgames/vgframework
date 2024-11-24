#pragma once

#include "renderer/IViewport.h"

namespace vg::gfx
{
    class Texture; 
}

namespace vg::renderer
{
    class View;
    class UIRenderer;

    //--------------------------------------------------------------------------------------
    // A Viewport is a collection of views rendered together
    //--------------------------------------------------------------------------------------
    class Viewport : public IViewport
    {
    public:
        const char * GetClassName() const override { return "Viewport"; }

        Viewport(const CreateViewportParams & _params);
        ~Viewport();

        void                                        SetRenderTargetSize (core::uint2 _size) final override;
        core::uint2                                 GetRenderTargetSize () const final override;

        void                                        SetRenderTarget     (gfx::ITexture * _renderTarget) final override;
        gfx::ITexture *                             GetRenderTarget     () const final override;

        void                                        SetFlags            (ViewportFlags _flagsToSet, ViewportFlags _flagsToRemove = (ViewportFlags)0x0) final override;
        ViewportFlags                               GetFlags            () const final override;

        void                                        SetViewportID       (gfx::ViewportID _viewportID) final override;
        gfx::ViewportID                             GetViewportID       () const final override;

        void                                        SetFocused          (bool _active) final override;
        bool                                        AnyFocused          () const final override;

        void                                        SetVisible          (bool _visible) final override;
        bool                                        AnyVisible          () const final override;

        void                                        SetRender           (bool _visible) final override;
        bool                                        AnyRender           () const final override;

        void                                        AddView             (core::u8 _index, gfx::ViewID _viewID) final override;
        void                                        RemoveView          (core::u8 _index, gfx::ViewID _viewID) final override;

        const core::map<core::u8, gfx::ViewID> &    GetViewIDs          () const final override;
        IUIRenderer *                               GetUIRenderer       () const final override;

        const core::string                          GetFrameGraphID     (const core::string & _name) const final override;

    private:
        gfx::ViewportID                             m_viewportID;
        ViewportFlags                               m_flags = (ViewportFlags)0x0;
        core::uint2                                 m_renderTargetSize = core::uint2(0, 0);
        gfx::Texture *                              m_renderTarget = nullptr;
        core::map<core::u8, gfx::ViewID>            m_viewIDs;
        UIRenderer *                                m_viewportGUI = nullptr;
    };
}