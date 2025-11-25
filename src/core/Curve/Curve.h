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

            virtual void            ResetBounds         () = 0;
            virtual void            FitVerticalBounds   () = 0;
            virtual bool            IsVisible           (uint _curveIndex) const = 0;
            virtual void            SetVisible          (uint _curveIndex, bool _visible) = 0;

            virtual void            OnLoad              () override;

            CurveType               getCurveValueType   () const { return m_type; }
            CurveInterpolationType  getInterpolationType() const { return m_interpolation; }

            const core::float2 &    getHorizontalRange  () const { return m_rangeX; }
            const core::float2 &    getVerticalRange    () const { return m_rangeY; }

            core::float2 &          getDisplayMin       () { return m_displayMin; }
            core::float2 &          getDisplayMax       () { return m_displayMax; }

        protected:
            CurveType               m_type = CurveType::Float;
            CurveInterpolationType  m_interpolation = CurveInterpolationType::Linear;
            core::float2            m_rangeX = core::float2(0, 1);
            core::float2            m_rangeY = core::float2(0, 1);

            // display range
            core::float2            m_displayMin = core::float2(0, 0);
            core::float2            m_displayMax = core::float2(1, 1);
    };
}