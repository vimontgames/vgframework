#pragma once

#include "core/IModel.h"

namespace vg::core
{
    class Model : public IModel
    {
    public:
        Model(const string & _name, IObject * _parent);
    };
}