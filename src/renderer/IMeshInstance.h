#pragma once
#include "renderer/Instance/GraphicInstance.h"

namespace vg::core
{
    class IWorld;
}

namespace vg::renderer
{
    class ISkeletalAnimation;
    class ISkeleton;

    enum class BodyPartFlags : core::u32;

    constexpr core::uint MaxAnimationLayerCount = 4;

    class IMeshInstance : public GraphicInstance
    {
    public:
        VG_CLASS_DECL_ABSTRACT(IMeshInstance, GraphicInstance);

        virtual const ISkeleton *   GetSkeleton             () const = 0;
        virtual bool                UpdateSkeleton          () = 0;
        virtual bool                DrawSkeleton            (const core::IWorld * _world) const = 0;

        virtual bool                AddAnimation            (ISkeletalAnimation * _animation) = 0;
        virtual bool                RemoveAnimation         (ISkeletalAnimation * _animation) = 0;

        virtual bool                SetAnimationLayer       (ISkeletalAnimation * _animation, core::uint _layer) = 0;
        virtual bool                SetAnimationBodyParts   (ISkeletalAnimation * _animation, BodyPartFlags _flags) = 0;

        virtual bool                SetAnimationTime        (ISkeletalAnimation * _animation, float _time) = 0;
        virtual bool                SetAnimationWeight      (ISkeletalAnimation * _animation, float _weight) = 0;

        virtual core::string        GetBatchName            (core::uint _batchIndex) const = 0;
    };
}