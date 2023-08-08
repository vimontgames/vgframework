#pragma once

#include "core/Model/Model.h"
#include "gfx/Resource/Texture_consts.h"

namespace vg::gfx
{
    class ITexture : public core::Model
    {
        using super = core::Model;

    public:
        ITexture(const core::string & _name, core::IObject * _parent) :
            super(_name, _parent)
        {

        }

        virtual core::u16 GetWidth() const = 0;
        virtual core::u16 GetHeight() const = 0;
    };
}