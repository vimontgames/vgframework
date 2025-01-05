#pragma once

#include "gfx/IFrameGraphView.h"

namespace vg::renderer
{
    class IViewport;

    struct CreateViewParams
    {
        CreateViewParams()
        {
        }

        CreateViewParams(gfx::ViewTarget _target, core::uint2 _size, IViewport * _viewport, core::IWorld * _world = nullptr, gfx::ITexture * _dest = nullptr) :
            viewport(_viewport),
            target(_target),
            size(_size),
            world(_world),
            dest(_dest)
        {
         
        }

        IViewport *     viewport = nullptr;
        gfx::ViewTarget target   = gfx::ViewTarget::Game;
        core::uint2     size     = core::uint2(0, 0);
        core::IWorld *  world    = nullptr;
        gfx::ITexture * dest     = nullptr;
    };

    struct ViewCullingStats
    {
        core::uint opaque      = 0;
        core::uint alphatest   = 0;
        core::uint transparent = 0;
        core::uint decal       = 0;
        core::uint selected    = 0;
        core::uint directional = 0;
        core::uint omni        = 0;
        core::uint spot        = 0;
    };

    vg_enum_class(ViewFlags, core::u32,
        Visible    = 0x00000001,
        Render     = 0x00000002,
        Focus      = 0x00000004,
        Ortho      = 0x00000008,
        Prefab     = 0x00000010,
        Additional = 0x00000020
    );

    vg_enum_class(ViewMask, core::u32,
        View_0  = 0x00000001,
        View_1  = 0x00000002,
        View_2  = 0x00000004,
        View_3  = 0x00000008,
        View_4  = 0x00000010,
        View_5  = 0x00000020,
        View_6  = 0x00000040,
        View_7  = 0x00000080,
        View_8  = 0x00000100,
        View_9  = 0x00000200,
        View_10 = 0x00000400,
        View_11 = 0x00000800,
        View_12 = 0x00001000,
        View_13 = 0x00002000,
        View_14 = 0x00004000,
        View_15 = 0x00008000
    );

    class IViewport;
    class IUIRenderer;
    class ICameraSettings;

    static const ViewMask ViewMask_All = (ViewMask)0x0000FFFF;

    class IView : public gfx::IFrameGraphView
    {
    public:
        virtual void                    SetupPerspectiveCamera      (const core::float4x4 & _cameraWorldMatrix, core::float2 _nearFar, float _fovY, core::float2 _viewportOffset = core::float2(0, 0), core::float2 _viewportScale = core::float2(1, 1)) = 0;
        virtual void                    SetupOrthographicCamera     (const core::float4x4 & _cameraWorldMatrix, core::uint2 _size, core::float2 _nearFar) = 0;
        virtual void                    SetupPhysicalCamera         (const core::float4x4 & _cameraWorldMatrix, const ICameraSettings * _cameraSettings, core::float2 _viewportOffset = core::float2(0, 0), core::float2 _viewportScale = core::float2(1, 1)) = 0;

        virtual void                    SetFlags                    (ViewFlags _flagsToSet, ViewFlags _flagsToRemove = (ViewFlags)0) = 0;
        virtual ViewFlags               GetFlags                    () const = 0;

        virtual const core::float4x4 &  GetViewInvMatrix            () const = 0;
        virtual const core::float4x4 &  GetViewMatrix               () const = 0;
        virtual const core::float4x4 &  GetProjectionMatrix         () const = 0;
        virtual core::float2            GetCameraNearFar            () const = 0;
        virtual float                   GetCameraFovY               () const = 0;

        virtual void                    SetWorld                    (core::IWorld * _world) = 0;
        virtual core::IWorld *          GetWorld                    () const = 0;

        virtual void                    SetRenderTargetSize         (core::uint2 _size) = 0;
        virtual core::uint2             GetRenderTargetSize         () const = 0;

        virtual core::uint2             GetSize                     () const = 0;
        virtual core::int2              GetOffset                   () const = 0;

        virtual core::float2            GetViewportOffset           () const = 0;
        virtual core::float2            GetViewportScale            () const = 0;
        virtual IViewport *             GetViewport                 () const = 0;

        virtual void                    SetRenderTarget             (gfx::ITexture * _renderTarget) = 0;
        virtual gfx::ITexture *         GetRenderTarget             () const = 0;

        // "Focused" means this is the view the user is interacting with in Editor
        virtual void                    SetFocused                  (bool _active) = 0;
        virtual bool                    IsFocused                   () const = 0;

        // "Visible" means the view is visible on screen
        virtual void                    SetVisible                  (bool _visible) = 0;
        virtual bool                    IsVisible                   () const = 0;

        // "Render" means the view was updated by an active camera this frame and thus should render this frame
        virtual void                    SetRender                   (bool _render) = 0;
        virtual bool                    IsRender                    () const = 0;

        virtual void                    SetMouseOffset              (const core::uint2 & _mouseOffset) = 0;
        virtual core::int2              GetRelativeMousePos         () const = 0;
        virtual bool                    IsMouseOverView             () const = 0;

        virtual const core::string      GetFrameGraphID             (const core::string & _name) const = 0;

        virtual bool                    IsToolmode                  () const = 0;
        virtual bool                    IsOrtho                     () const = 0;
        virtual bool                    IsLit                       () const = 0;
        virtual bool                    IsUsingRayTracing           () const = 0;
        virtual bool                    IsComputePostProcessNeeded  () const = 0;
      
        virtual void                    SetPickingData              (const PickingData & _pickingData) = 0;
        virtual const PickingHit &      GetPickingHit               (core::uint _index) const = 0;
        virtual core::uint              GetPickingHitCount          () const = 0;
        virtual core::uint              GetPickingRequestedHitCount () const = 0;
        virtual const PickingHit &      GetPickingClosestHit        () const = 0;
        virtual void                    AddPickingHit               (const PickingHit & _hit) = 0;

        virtual ViewCullingStats        GetViewCullingStats         () const = 0;
        virtual IUIRenderer *           GetUIRenderer               () const = 0;
        virtual const ICameraSettings * GetCameraSettings           () const = 0;
    };
}