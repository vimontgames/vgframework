#pragma once

#include "Curve.h"

namespace vg::core
{
    struct FloatCurveData 
    {
        struct FloatCurvePoint
        {
            float t;
            float value;
        };
        core::vector<FloatCurvePoint> m_points;
    };

    class FloatCurveDataContainer : Object
    {
    public:
        VG_CLASS_DECL(FloatCurveDataContainer, Object);

        FloatCurveDataContainer(const string & _name = "", IObject * _parent = nullptr);
        ~FloatCurveDataContainer();

        FloatCurveData m_curve;
    };

    class FloatCurve final : public Curve
    {
    public:
        VG_CLASS_DECL(FloatCurve, Curve);

        FloatCurve(const string & _name = "", IObject * _parent = nullptr);
        ~FloatCurve();

        void OnPropertyChanged(IObject * _object, const IProperty & _prop, bool _notifyParent) final override;
        void Fit() final override;

        const uint getCurveCount() const { return (uint)m_data.size(); }
        const FloatCurveData & getCurveData(uint _index) const { return m_data[_index].m_curve; }
        FloatCurveData & getCurveData(uint _index) { return m_data[_index].m_curve; }

    private:
        core::vector<FloatCurveDataContainer> m_data;
    };
}