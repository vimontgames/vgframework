#pragma once

#include "core/Model/Model.h"

namespace vg::renderer
{
    class ICameraLens : public core::Model
    {
    public:
        VG_CLASS_DECL_ABSTRACT(ICameraLens, core::Model);

        virtual const core::float2 &    GetSensorSize               () const = 0;
        virtual const core::float2 &    GetFocalLengthRange         () const = 0;
        virtual const core::float2 &    GetMinFocusDistanceRange    () const = 0;
        virtual const core::float2 &    GetMinApertureRange         () const = 0;
        virtual const core::float2 &    GetMaxApertureRange         () const = 0;

        virtual float                   GetZoomFactor               (float _focalLength) const = 0;
        virtual float                   GetMinFocusDistance         (float _focalLength) const = 0;
        virtual float                   GetMinAperture              (float _focalLength) const = 0;
        virtual float                   GetMaxAperture              (float _focalLength) const = 0;
    };
}