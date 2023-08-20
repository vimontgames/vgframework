#pragma once

#include "core/Instance/Instance.h"

namespace vg::gfx
{
    class CommandList;
}

namespace vg::renderer
{
    class RenderContext;

    class IGraphicInstance : public core::Instance
    {
    public:
        IGraphicInstance(const core::string & _name, core::IObject * _parent) :
            core::Instance(_name, _parent)
        {

        }

        virtual void Draw(const RenderContext & _renderContext, gfx::CommandList * _cmdList) const = 0;
    };
}