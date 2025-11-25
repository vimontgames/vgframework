#include "FloatCurve.h"
#include "core/Math/Math.h"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(FloatCurveDataContainer, "Float curve data container");

    //--------------------------------------------------------------------------------------
    FloatCurveDataContainer::FloatCurveDataContainer(const string & _name, IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    FloatCurveDataContainer::~FloatCurveDataContainer()
    {

    }

    //--------------------------------------------------------------------------------------
    void * ResizeFloatCurveDataContainerVector(IObject * _parent, uint _offset, uint _count)
    {
        auto vec = (core::vector<FloatCurveDataContainer> *)(uint_ptr(_parent) + _offset);
        vec->clear();
        vec->resize(_count);
        return vec->data();
    }

    //--------------------------------------------------------------------------------------
    bool FloatCurveDataContainer::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(FloatCurveDataContainer, m_curve, "Curve");
        registerProperty(FloatCurveDataContainer, m_isVisible, "Visible");
        registerResizeVectorFunc(FloatCurveDataContainer, ResizeFloatCurveDataContainerVector);

        return true;
    }

    VG_REGISTER_OBJECT_CLASS(FloatCurve, "Float curve");

    //--------------------------------------------------------------------------------------
    FloatCurve::FloatCurve(const string & _name, IObject * _parent) :
        super(_name, _parent)
    {
        FloatCurveDataContainer & curve = m_data.emplace_back();
        curve.RegisterUID();

        curve.m_curve.m_points.push_back({ 0.0f, 0.0f });
        curve.m_curve.m_points.push_back({ 1.0f, 1.0f });
    }

    //--------------------------------------------------------------------------------------
    FloatCurve::~FloatCurve()
    {

    }

    //--------------------------------------------------------------------------------------
    bool FloatCurve::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyObjectVector(FloatCurve, m_data, FloatCurveDataContainer, "Data");

        return true;
    }

    //--------------------------------------------------------------------------------------
    void FloatCurve::OnPropertyChanged(IObject * _object, const IProperty & _prop, bool _notifyParent)
    {
        if (!strcmp(_prop.GetName(), "m_type"))
        {
            CurveType curveType = *_prop.GetPropertyEnum<CurveType>(_object);

            switch (curveType)
            {
                case CurveType::Float:
                    m_data.resize(1);
                    break;

                case CurveType::Float4:
                    m_data.resize(4);
                    break;
            }

            for (uint i = 0; i < m_data.size(); ++i)
            {
                auto & curve = m_data[i];
                if (curve.m_curve.m_points.size() == 0)
                {
                    curve.m_curve.m_points.push_back({ 0.0f, 0.0f });
                    curve.m_curve.m_points.push_back({ 1.0f, 1.0f });
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void FloatCurve::ResetBounds()
    {
        m_displayMin.x = m_rangeX.x;
        m_displayMax.x = m_rangeX.y;

        m_displayMin.y = m_rangeY.x;
        m_displayMax.y = m_rangeY.y;
    }

    //--------------------------------------------------------------------------------------
    void FloatCurve::FitVerticalBounds()
    {
        ResetBounds();

        float2 minimum = (float2)FLT_MAX;
        float2 maximum = (float2)-FLT_MAX;

        for (uint i = 0; i < m_data.size(); ++i)
        {
            auto & curve = m_data[i];
            auto & points = curve.m_curve.m_points;
            for (uint j = 0; j < points.size(); ++j)
            {
                minimum.xy = min(minimum.xy, float2(points[j].t, points[j].value));
                maximum.xy = max(maximum.xy, float2(points[j].t, points[j].value));
            }
        }

        m_displayMin.y = minimum.y;
        m_displayMax.y = maximum.y;

        const float eps = 0.1f;
        if (abs((float)m_displayMin.x - (float)m_displayMax.x) < eps)
        {
            m_displayMin.x -= eps / 2.0f;
            m_displayMax.x += eps / 2.0f;
        }

        if (abs((float)m_displayMin.y - (float)m_displayMax.y) < eps)
        {
            m_displayMin.y -= eps / 2.0f;
            m_displayMax.y += eps / 2.0f;
        }
    }

    //--------------------------------------------------------------------------------------
    bool FloatCurve::IsVisible(uint _curveIndex) const
    {
        if (_curveIndex < m_data.size())
            return m_data[_curveIndex].m_isVisible;
        else
            return false;
    }

    //--------------------------------------------------------------------------------------
    void FloatCurve::SetVisible(uint _curveIndex, bool _visible)
    {
        if (_curveIndex < m_data.size())
            m_data[_curveIndex].m_isVisible = _visible;
    }

    //--------------------------------------------------------------------------------------
    float FloatCurve::getValue(float _time, uint _curveIndex) const
    {
        if (_curveIndex < m_data.size())
        {
            const FloatCurveDataContainer & curve = m_data[_curveIndex];
            return curve.m_curve.getValue(_time, getInterpolationType());
        }

        return 0.0f;
    }

    //--------------------------------------------------------------------------------------
    float FloatCurveData::getValue(float _time, CurveInterpolationType _interpolationType) const
    {
        const size_t count = m_points.size();

        // No points -> return 0
        if (count == 0)
            return 0.0f;

        // Single point -> always return its value
        if (count == 1)
            return m_points[0].value;

        // Before first point -> clamp
        if (_time <= m_points.front().t)
            return m_points.front().value;

        // After last point -> clamp
        if (_time >= m_points.back().t)
            return m_points.back().value;

        // Find the segment that contains _time
        // (simple linear search; can be optimized with binary search if needed)
        size_t i = 0;
        for (; i < count - 1; ++i)
        {
            const FloatCurvePoint & p0 = m_points[i];
            const FloatCurvePoint & p1 = m_points[i + 1];

            if (_time >= p0.t && _time <= p1.t)
            {
                const float dt = p1.t - p0.t;
                const float alpha = (_time - p0.t) / dt;

                switch (_interpolationType)
                {
                    case CurveInterpolationType::Constant:
                        // Step interpolation
                        return p0.value;

                    default:
                    case CurveInterpolationType::Linear:
                        // Standard lerp
                        return p0.value + (p1.value - p0.value) * alpha;

                    case CurveInterpolationType::Smooth:
                    {
                        // Smoothstep interpolation
                        float s = alpha * alpha * (3.0f - 2.0f * alpha);
                        return p0.value + (p1.value - p0.value) * s;
                    }
                }
            }
        }

        // Should never reach here, but just in case
        return m_points.back().value;
    }
}