#pragma once
#include "core/Resource/Resource.h"

namespace vg::engine
{
    class IAnimationResource : public core::Resource
    {
        public:
            VG_CLASS_DECL_ABSTRACT(IAnimationResource, core::Resource);

            virtual bool IsPlaying  () const = 0;
            virtual bool IsFinished () const = 0;

            virtual bool PlayLoop   () = 0;
            virtual bool PlayOnce   () = 0;

            virtual bool Stop       () = 0;

    };
}