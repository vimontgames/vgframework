#pragma once

#include "Curve.h"

namespace vg::core
{
    struct FloatCurveData 
    {
        float getValue(float _time, CurveInterpolationType _interpolationType) const;

        struct FloatCurvePoint
        {
            float t;
            float value;

            bool operator != (const FloatCurvePoint & _other) const
            {
                return t != _other.t || value != _other.value;
            }
        };

        bool operator != (const FloatCurveData & _other) const
        {
            if (m_points.size() != _other.m_points.size())
                return true;

            for (uint i = 0; i < m_points.size(); ++i)
            {
                if (m_points[i] != _other.m_points[i])
                    return true;
            }

            return false;
        }

        core::vector<FloatCurvePoint> m_points;
    };

    class FloatCurveDataContainer : public Object
    {
    public:
        VG_CLASS_DECL(FloatCurveDataContainer, Object);

        FloatCurveDataContainer(const string & _name = "", IObject * _parent = nullptr);
        ~FloatCurveDataContainer();

        bool operator != (const FloatCurveDataContainer & _other) const
        {
            if (m_curve != _other.m_curve)
                return true;

            return false;
        }

        FloatCurveData m_curve;
        bool m_isVisible = true;
    };

    class FloatCurve final : public Curve
    {
    public:
        VG_CLASS_DECL(FloatCurve, Curve);

        FloatCurve(const string & _name = "", IObject * _parent = nullptr);
        ~FloatCurve();

        bool operator != (const FloatCurve & _other) const
        {
            if (m_data.size() != _other.m_data.size())
                return true;

            for (uint i = 0; i < getCurveCount(); ++i)
            {
                if (m_data[i] != _other.m_data[i])
                    return true;
            }

            return false;
        }

        void                    OnPropertyChanged   (IObject * _object, const IProperty & _prop, bool _notifyParent) final override;

        void                    ResetBounds         () final override;
        void                    FitVerticalBounds   () final override;
        bool                    IsVisible           (uint _curveIndex) const final override;
        void                    SetVisible          (uint _curveIndex, bool _visible) final override;

        float                   getValue            (float _time, uint _curveIndex = 0) const;
        const uint              getCurveCount       () const { return (uint)m_data.size(); }
        const FloatCurveData &  getCurveData        (uint _index) const { return m_data[_index].m_curve; }
        FloatCurveData &        getCurveData        (uint _index) { return m_data[_index].m_curve; }

    private:
        core::vector<FloatCurveDataContainer> m_data;
    };
}