#pragma once

#include "IGraphicInstance.h"

namespace vg::renderer
{
    class IMeshInstance : public IGraphicInstance
    {
    public:
        IMeshInstance(const core::string & _name, core::IObject * _parent) :
            IGraphicInstance(_name, _parent)
        {

        }
    };
}