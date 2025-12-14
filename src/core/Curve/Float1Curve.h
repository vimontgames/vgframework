#pragma once

#include "FloatNCurve.h"

namespace vg::core
{
    class Float1Curve final : public FloatNCurve
    {
    public:
        VG_CLASS_DECL(Float1Curve, FloatNCurve);

        Float1Curve(const string & _name = "", IObject * _parent = nullptr);
        ~Float1Curve();

        bool CanChangeCurveCount() const { return false; }
        float getValue(float _time) const { return super::getFloatValue(_time); }
    };
}