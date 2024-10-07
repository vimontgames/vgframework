#pragma once
#include "core/Resource/Resource.h"

namespace vg::engine
{
    enum class AnimationFlags : core::u32
    {
        Play = 0x00000001,
        Loop = 0x00000002
    };

    class IAnimationResource : public core::Resource
    {
        public:
            VG_CLASS_DECL_ABSTRACT(IAnimationResource, core::Resource);

            virtual bool    IsPlaying   () const = 0;
            virtual bool    IsFinished  () const = 0;

            virtual bool    PlayLoop    () = 0;
            virtual bool    PlayOnce    () = 0;

            virtual bool    Stop        () = 0;

            virtual float   GetTime     () const = 0;
            virtual float   GetLength   () const = 0;
            virtual float   GetWeight   () const = 0;
    };
}