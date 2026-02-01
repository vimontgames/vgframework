#pragma once

#include "FloatNCurve.h"

namespace vg::core
{
    class Float2Curve final : public FloatNCurve
    {
    public:
        VG_CLASS_DECL(Float2Curve, FloatNCurve);

        Float2Curve(const string & _name = "", IObject * _parent = nullptr);
        ~Float2Curve();

        bool CanChangeCurveCount() const override { return false; }
        core::float2 getValue(float _time) const { return super::getFloat2Value(_time); }
    };
}