#include "Float1Curve.h"

namespace vg::core
{
    VG_REGISTER_CLASS(Float1Curve, "Float1 curve");

    //--------------------------------------------------------------------------------------
    Float1Curve::Float1Curve(const string & _name, IObject * _parent) :
        super(_name, _parent)
    {
        SetCurveCount(1);
    }

    //--------------------------------------------------------------------------------------
    Float1Curve::~Float1Curve()
    {

    }

    //--------------------------------------------------------------------------------------
    bool Float1Curve::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        return true;
    }
}