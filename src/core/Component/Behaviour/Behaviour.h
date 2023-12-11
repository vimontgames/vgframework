#pragma once
#include "core/IBehaviour.h"

namespace vg::core
{
    class Behaviour : public IBehaviour
    {
    public:
        VG_CLASS_DECL(Behaviour, IBehaviour);

        Behaviour(const core::string & _name, IObject * _parent);
        ~Behaviour();
    };
}