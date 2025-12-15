#include "core/Precomp.h"
#include "Curve.h"
#include "core/Object/AutoRegisterClass.h"
#include "core/Kernel.h"

#include "FloatNCurve.hpp"
#include "Float1Curve.hpp"
#include "Float2Curve.hpp"
#include "Float3Curve.hpp"
#include "Float4Curve.hpp"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    bool IsCurveTypeHidden(const IObject * _object, const IProperty * _prop, uint _index)
    {
        const Curve * curve = VG_SAFE_STATIC_CAST(const Curve, _object);
        return !curve->CanChangeCurveCount();
    }

    //--------------------------------------------------------------------------------------
    Curve::Curve(const string & _name, IObject * _parent) :
        super(_name, _parent)
    {
        RegisterUID();
    }

    //--------------------------------------------------------------------------------------
    Curve::~Curve()
    {
        UnregisterUID();
    }

    //--------------------------------------------------------------------------------------
    bool Curve::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEnum(Curve, CurveType, m_type, "Value type");
        setPropertyHiddenCallback(Curve, m_type, IsCurveTypeHidden);

        registerPropertyEnum(Curve, CurveInterpolationType, m_interpolation, "Interpolation");

        // Range
        registerProperty(Curve, m_rangeX, "Time");
        registerProperty(Curve, m_rangeY, "Values");
 
        return true;
    }

    //--------------------------------------------------------------------------------------
    void Curve::Swap(IObject * _other)
    {
        super::Swap(_other);

        Curve * other = VG_SAFE_STATIC_CAST(Curve, _other);

        swap(m_type, other->m_type);
        swap(m_valueType, other->m_valueType);
        swap(m_interpolation, other->m_interpolation);
        swap(m_rangeX, other->m_rangeX);
        swap(m_rangeY, other->m_rangeY);
        swap(m_displayMin, other->m_displayMin);
        swap(m_displayMax, other->m_displayMax);
    }

    //--------------------------------------------------------------------------------------
    void Curve::OnLoad()
    {
        ResetBounds();
    }
}