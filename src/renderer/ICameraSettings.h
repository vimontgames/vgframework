#pragma once

#include "core/Object/Object.h"
#include "renderer/IView.h"

namespace vg::renderer
{
    class ICameraSettings : public core::Object
    {
    public:
        VG_CLASS_DECL_ABSTRACT(ICameraSettings, core::Object);

        virtual const ICameraLens * GetCameraLens   () const = 0;
        virtual float               GetFocalLength  () const = 0;
        virtual GateFitMode    GetGateFitMode  () const = 0;
        virtual float               GetNear         () const = 0;
        virtual float               GetFar          () const = 0;
    };
}