#include "Float3Curve.h"

namespace vg::core
{
    VG_REGISTER_CLASS(Float3Curve, "Float3 curve");

    //--------------------------------------------------------------------------------------
    Float3Curve::Float3Curve(const string & _name, IObject * _parent) :
        super(_name, _parent)
    {
        SetCurveCount(3);
    }

    //--------------------------------------------------------------------------------------
    Float3Curve::~Float3Curve()
    {

    }

    //--------------------------------------------------------------------------------------
    bool Float3Curve::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        return true;
    }
}