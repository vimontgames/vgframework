#pragma once

#include "FloatNCurve.h"

namespace vg::core
{
    class Float3Curve final : public FloatNCurve
    {
    public:
        VG_CLASS_DECL(Float3Curve, FloatNCurve);

        Float3Curve(const string & _name = "", IObject * _parent = nullptr);
        ~Float3Curve();

        bool CanChangeCurveCount() const { return false; }
        core::float3 getValue(float _time) const { return super::getFloat3Value(_time); }
    };
}