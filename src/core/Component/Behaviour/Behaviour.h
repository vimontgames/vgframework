#pragma once
#include "core/IBehaviour.h"

namespace vg::core
{
    class Behaviour : public IBehaviour
    {
    public:
        static bool     registerProperties  (IClassDesc & _desc);

                        Behaviour           (const core::string & _name, IObject * _parent);
                        ~Behaviour          ();
    };
}