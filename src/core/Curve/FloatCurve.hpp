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
        registerResizeVectorFunc(WorldResource, ResizeFloatCurveDataContainerVector);

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
    void FloatCurve::Fit()
    {
        m_displayMin.x = m_range.x;
        m_displayMax.x = m_range.y;

        float minY = FLT_MAX;
        float maxY = -FLT_MAX;

        for (uint i = 0; i < m_data.size(); ++i)
        {
            auto & curve = m_data[i];
            auto & points = curve.m_curve.m_points;
            for (uint j = 0; j < points.size(); ++j)
            {
                minY = min(minY, points[j].value);
                maxY = max(maxY, points[j].value);
            }
        }

        m_displayMin.y = minY;
        m_displayMax.y = maxY;

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
}