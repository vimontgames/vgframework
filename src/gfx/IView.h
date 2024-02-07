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
    class UserPass;

    enum class ViewTarget : core::u8
    {
        Backbuffer  = 0,
        Game        = 1,
        Editor      = 2,
        Shadow      = 3
    };
    static inline const ViewTarget ViewTargetInvalid = (ViewTarget)-1;

    using ViewIndex = core::u8;
    static inline const ViewIndex ViewIndexInvalid = (ViewIndex)-1;

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

        CreateViewParams(ViewTarget _target, core::uint2 _size, core::int2 _offset = core::int2(0, 0), core::IWorld * _world = nullptr, gfx::ITexture * _dest = nullptr) :
            target(_target),
            size(_size),
            offset(_offset),
            world(_world),
            dest(_dest)
        {
         
        }

        ViewTarget          target = ViewTarget::Game;
        core::uint2         size = core::uint2(0, 0);
        core::int2          offset = core::int2(0, 0);
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

    class IView : public core::Object
    {
    public:
        enum class Flags : core::u32
        {
            Picking = 0x00000001,
            Prefab  = 0x00000002
        };

        IView() {};
        virtual ~IView() = default;

        virtual void                    SetupPerspectiveCamera      (const core::float4x4 & _cameraWorldMatrix, core::float2 _nearFar, float _fovY) = 0;
        virtual void                    SetupOrthographicCamera     (const core::float4x4 & _cameraWorldMatrix, core::uint2 _size, core::float2 _nearFar) = 0;

        virtual void                    SetFlags                    (Flags _flagsToSet, Flags _flagsToRemove = (Flags)0) = 0;
        virtual Flags                   GetFlags                    () const = 0;

        virtual const core::float4x4 &  GetViewInvMatrix            () const = 0;
        virtual const core::float4x4 &  GetProjectionMatrix         () const = 0;
        virtual core::float2            GetCameraNearFar            () const = 0;
        virtual float                   GetCameraFovY               () const = 0;

        virtual void                    SetWorld                    (core::IWorld * _world) = 0;
        virtual core::IWorld *          GetWorld                    () const = 0;

        virtual void                    SetSize                     (core::uint2 _size) = 0;
        virtual core::uint2             GetSize                     () const = 0;

        virtual void                    SetOffset                   (core::int2) = 0;
        virtual core::int2              GetOffset                   () const = 0;

        virtual void                    SetRenderTarget             (gfx::ITexture * _renderTarget) = 0;
        virtual gfx::ITexture *         GetRenderTarget             () const = 0;

        virtual void                    SetViewID                   (ViewID _viewID) = 0;
        virtual ViewID                  GetViewID                   () const = 0;

        virtual void                    SetActive                   (bool _active) = 0;
        virtual bool                    IsActive                    () const = 0;

        virtual void                    SetVisible                  (bool _visible) = 0;
        virtual bool                    IsVisible                   () const = 0;

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

        virtual ViewCullingStats        GetViewCullingStats         () const = 0;
    };
}