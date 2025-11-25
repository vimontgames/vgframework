#include "core/Precomp.h"
#include "Curve.h"
#include "core/Object/AutoRegisterClass.h"

#include "FloatCurve.hpp"

namespace vg::core
{
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
        registerPropertyEnum(Curve, CurveInterpolationType, m_interpolation, "Interpolation");
        registerProperty(Curve, m_rangeX, "Horizontal range");
        registerProperty(Curve, m_rangeY, "Vertical range");

        return true;
    }

    //--------------------------------------------------------------------------------------
    void Curve::OnLoad()
    {
        ResetBounds();
    }
}