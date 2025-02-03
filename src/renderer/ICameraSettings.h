#pragma once

#include "core/Object/Object.h"
#include "renderer/IView.h"

vg_enum_class_ns(vg::renderer, GateFitMode, core::u8,
    Horizontal,
    Vertical
);

namespace vg::renderer
{
    class ICameraSettings : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(ICameraSettings, core::Object);

        virtual const ICameraLens * GetCameraLens           () const = 0;
        virtual float               GetFocalLength          () const = 0;
        virtual float               GetAperture             () const = 0;
        virtual float               GetFocusDistance        () const = 0;
        virtual GateFitMode         GetGateFitMode          () const = 0;
        virtual float               GetNear                 () const = 0;
        virtual float               GetFar                  () const = 0;

        virtual bool                IsDepthOfFieldEnabled   () const = 0;
        virtual bool                IsMotionBlurEnabled     () const = 0;
    };
}