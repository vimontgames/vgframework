#include "FloatNCurve.h"
#include "core/Math/Math.h"

namespace vg::core
{
    VG_REGISTER_OBJECT_CLASS(FloatCurveDataContainer, "Float curve data container");

    //--------------------------------------------------------------------------------------
    FloatCurveDataContainer::FloatCurveDataContainer(const string & _name, IObject * _parent) :
        super(_name, _parent)
    {
        RegisterUID();
    }

    //--------------------------------------------------------------------------------------
    FloatCurveDataContainer::~FloatCurveDataContainer()
    {
        UnregisterUID();
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

    VG_REGISTER_OBJECT_CLASS(FloatNCurve, "Float curve");

    //--------------------------------------------------------------------------------------
    FloatNCurve::FloatNCurve(const string & _name, IObject * _parent) :
        super(_name, _parent)
    {
        FloatCurveDataContainer & curve = m_data.emplace_back();

        curve.m_curve.m_points.push_back({ 0.0f, 0.0f });
        curve.m_curve.m_points.push_back({ 1.0f, 1.0f });
    }

    //--------------------------------------------------------------------------------------
    FloatNCurve::~FloatNCurve()
    {

    }

    //--------------------------------------------------------------------------------------
    void FloatNCurve::Swap(IObject * _other)
    {
        super::Swap(_other);
        FloatNCurve & other = *VG_SAFE_STATIC_CAST(FloatNCurve, _other);
        swap(m_data, other.m_data);
    }

    //--------------------------------------------------------------------------------------
    bool FloatNCurve::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyObjectVector(FloatNCurve, m_data, FloatCurveDataContainer, "Data");

        return true;
    }

    //--------------------------------------------------------------------------------------
    void FloatNCurve::SetCurveCount(core::uint _curveCount)
    {
        switch (_curveCount)
        {
            case 1:
                m_type = CurveType::Float;
            break;

            case 2:
                m_type = CurveType::Float2;
            break;

            case 3:
                m_type = CurveType::Float3;
            break;

            case 4:
                m_type = CurveType::Float4;
            break;

            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED((CurveType)_curveCount);
            return;
        }

        if (m_data.size() != _curveCount)
            m_data.resize(_curveCount);

        for (uint i = 0; i < m_data.size(); ++i)
        {
            auto & curve = m_data[i];
            curve.RegisterUID();
            if (curve.m_curve.m_points.size() == 0)
            {
                curve.m_curve.m_points.push_back({ 0.0f, 0.0f });
                curve.m_curve.m_points.push_back({ 1.0f, 1.0f });
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void FloatNCurve::OnPropertyChanged(IObject * _object, const IProperty & _prop, bool _notifyParent)
    {
        if (!strcmp(_prop.GetName(), "m_type"))
        {
            CurveType curveType = *_prop.GetPropertyEnum<CurveType>(_object);

            switch (curveType)
            {
                case CurveType::Float:
                    SetCurveCount(1);
                    break;

                case CurveType::Float2:
                    SetCurveCount(2);
                    break;

                case CurveType::Float3:
                    SetCurveCount(3);
                    break;

                case CurveType::Float4:
                    SetCurveCount(4);
                    break;
            }            
        }
    }

    //--------------------------------------------------------------------------------------
    void FloatNCurve::ResetBounds()
    {
        m_displayMin.x = m_rangeX.x;
        m_displayMax.x = m_rangeX.y;

        m_displayMin.y = m_rangeY.x;
        m_displayMax.y = m_rangeY.y;
    }

    //--------------------------------------------------------------------------------------
    void FloatNCurve::FitVerticalBounds()
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
    bool FloatNCurve::IsVisible(uint _curveIndex) const
    {
        if (_curveIndex < m_data.size())
            return m_data[_curveIndex].m_isVisible;
        else
            return false;
    }

    //--------------------------------------------------------------------------------------
    void FloatNCurve::SetVisible(uint _curveIndex, bool _visible)
    {
        if (_curveIndex < m_data.size())
            m_data[_curveIndex].m_isVisible = _visible;
    }

    //--------------------------------------------------------------------------------------
    float FloatNCurve::getCurveValue(float _time, uint _curveIndex) const
    {
        if (_curveIndex < m_data.size())
        {
            const FloatCurveDataContainer & curve = m_data[_curveIndex];
            return curve.m_curve.getValue(_time, getInterpolationType());
        }

        return 0.0f;
    }

    //--------------------------------------------------------------------------------------
    float FloatNCurve::getFloatValue(float _time) const
    {
        VG_ASSERT(m_data.size() == 1);
        return m_data[0].m_curve.getValue(_time, getInterpolationType());
    }

    //--------------------------------------------------------------------------------------
    core::float2 FloatNCurve::getFloat2Value(float _time) const
    {
        VG_ASSERT(m_data.size() == 2);
        const auto interpolation = getInterpolationType();

        return float2(
            m_data[0].m_curve.getValue(_time, interpolation),
            m_data[1].m_curve.getValue(_time, interpolation)
        );
    }

    //--------------------------------------------------------------------------------------
    core::float3 FloatNCurve::getFloat3Value(float _time) const
    {
        VG_ASSERT(m_data.size() == 3);
        const auto interpolation = getInterpolationType();

        return float3(
            m_data[0].m_curve.getValue(_time, interpolation),
            m_data[1].m_curve.getValue(_time, interpolation),
            m_data[2].m_curve.getValue(_time, interpolation)
        );
    }

    //--------------------------------------------------------------------------------------
    core::float4 FloatNCurve::getFloat4Value(float _time) const
    {
        VG_ASSERT(m_data.size() == 4);
        const auto interpolation = getInterpolationType();

        return float4(
            m_data[0].m_curve.getValue(_time, interpolation),
            m_data[1].m_curve.getValue(_time, interpolation),
            m_data[2].m_curve.getValue(_time, interpolation),
            m_data[3].m_curve.getValue(_time, interpolation)
        );
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