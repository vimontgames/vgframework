#pragma once

#include "FloatNCurve.h"

namespace vg::core
{
    class Float4Curve final : public FloatNCurve
    {
    public:
        VG_CLASS_DECL(Float4Curve, FloatNCurve);

        Float4Curve(const string & _name = "", IObject * _parent = nullptr);
        ~Float4Curve();

        bool CanChangeCurveCount() const override { return false; }
        core::float4 getValue(float _time) const { return super::getFloat4Value(_time); }
    };
}