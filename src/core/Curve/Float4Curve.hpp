#include "Float4Curve.h"

namespace vg::core
{
    VG_REGISTER_CLASS(Float4Curve, "Float4 curve");

    //--------------------------------------------------------------------------------------
    Float4Curve::Float4Curve(const string & _name, IObject * _parent) :
        super(_name, _parent)
    {
        SetCurveCount(4);
    }

    //--------------------------------------------------------------------------------------
    Float4Curve::~Float4Curve()
    {

    }

    //--------------------------------------------------------------------------------------
    bool Float4Curve::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        return true;
    }
}