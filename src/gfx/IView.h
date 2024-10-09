#pragma once

#include "core/Object/Object.h"
#include "core/IWorld.h"

struct PickingData;
struct PickingHit;

namespace vg::core
{
    class IGameObject;
}

namespace vg::gfx
{
    class ITexture;
    class IUIRenderer;
    class UserPass;
    class IViewport;
    struct ViewportID;

    vg_enum_class(ViewTarget, core::u8,
        Game        = 0,
        Editor      = 1,
        Shadow      = 2
    );
    static inline const ViewTarget ViewTargetInvalid = (ViewTarget)0x03;

    using ViewIndex = core::u8;
    static inline const ViewIndex ViewIndexInvalid = (ViewIndex)0x3F;

    struct ViewID
    {
        ViewID(ViewTarget _target = ViewTargetInvalid, ViewIndex _index = ViewIndexInvalid) :
            target(_target),
            index(_index)
        {
            
        }

        union
        {
            struct
            {
                ViewTarget target : 2;
                ViewIndex index : 6;
            };
            core::u8 id;
        };

        inline bool operator == (const ViewID & _other) const
        {
            return target == _other.target && index == _other.index;
        }

        inline bool operator != (const ViewID & _other) const
        {
            return !(*this == _other);
        }
    };

    struct CreateViewParams
    {
        CreateViewParams()
        {
        }

        CreateViewParams(ViewTarget _target, core::uint2 _size, IViewport * _viewport, core::IWorld * _world = nullptr, gfx::ITexture * _dest = nullptr) :
            viewport(_viewport),
            target(_target),
            size(_size),
            world(_world),
            dest(_dest)
        {
         
        }

        IViewport *         viewport = nullptr;
        ViewTarget          target = ViewTarget::Game;
        core::uint2         size = core::uint2(0, 0);
        core::IWorld *      world = nullptr;
        gfx::ITexture *     dest = nullptr;
    };

    struct ViewCullingStats
    {
        core::uint opaque       = 0;
        core::uint transparent  = 0;
        core::uint directional  = 0;
        core::uint omni         = 0;
        core::uint spot         = 0;
    };

    vg_enum_class(ViewFlags, core::u32,
        Visible     = 0x00000001,
        Render      = 0x00000002,
        Focus       = 0x00000004,
        Ortho       = 0x00000008,
        Prefab      = 0x00000010,
        Additional  = 0x00000020
    );

    class IView : public core::Object
    {
    public:

        IView() {};
        virtual ~IView() = default;

        virtual void                    SetupPerspectiveCamera      (const core::float4x4 & _cameraWorldMatrix, core::float2 _nearFar, float _fovY, core::float2 _viewportOffset, core::float2 _viewportScale) = 0;
        virtual void                    SetupOrthographicCamera     (const core::float4x4 & _cameraWorldMatrix, core::uint2 _size, core::float2 _nearFar) = 0;

        virtual void                    SetFlags                    (ViewFlags _flagsToSet, ViewFlags _flagsToRemove = (ViewFlags)0) = 0;
        virtual ViewFlags               GetFlags                    () const = 0;

        virtual const core::float4x4 &  GetViewInvMatrix            () const = 0;
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
        virtual ITexture *              GetRenderTarget             () const = 0;

        virtual void                    SetViewID                   (ViewID _viewID) = 0;
        virtual ViewID                  GetViewID                   () const = 0;

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

        virtual IUIRenderer *           GetUIRenderer                  () const = 0;
    };
}