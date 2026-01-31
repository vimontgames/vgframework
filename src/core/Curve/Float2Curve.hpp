#include "Float2Curve.h"

namespace vg::core
{
    VG_REGISTER_CLASS(Float2Curve, "Float2 curve");

    //--------------------------------------------------------------------------------------
    Float2Curve::Float2Curve(const string & _name, IObject * _parent) :
        super(_name, _parent)
    {
        SetCurveCount(2);
    }

    //--------------------------------------------------------------------------------------
    Float2Curve::~Float2Curve()
    {

    }

    //--------------------------------------------------------------------------------------
    bool Float2Curve::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        return true;
    }
}