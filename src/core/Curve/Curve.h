#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    vg_enum_class(vg::core, CurveType, u8,
        Float = 0,
        Float4
    );

    vg_enum_class(vg::core, CurveInterpolationType, u8,
        Constant = 0,
        Linear,
        Smooth
    );

    class Curve : public core::Object
    {
        public:
            VG_CLASS_DECL(Curve, core::Object);

            Curve(const string & _name = "", IObject * _parent = nullptr);
            ~Curve();

            virtual void Fit() = 0;

            CurveInterpolationType getInterpolationType() const { return m_interpolation; }
            core::float2 getRange() const { return m_range; }

            core::float2 & getDisplayMin() { return m_displayMin; }
            core::float2 & getDisplayMax() { return m_displayMax; }

        protected:
            CurveType               m_type = CurveType::Float;
            CurveInterpolationType  m_interpolation = CurveInterpolationType::Linear;
            core::float2            m_range = core::float2(0, 1);

            // display range
            core::float2            m_displayMin = core::float2(0, 0);
            core::float2            m_displayMax = core::float2(1, 1);
    };
}