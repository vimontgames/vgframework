#pragma once

#include "core/Object/Object.h"

namespace vg::engine
{
    class IResourceManager : public core::Object
    {
    public:
        using super = core::Object;

        IResourceManager(const core::string & _name, core::IObject * _parent) :
            super(_name, _parent)
        {

        }
        
    };
}