#pragma once

#include "core/Model/Model.h"

namespace vg::graphics
{
    namespace driver
    {
        class ITexture;
    }

    namespace renderer
    {
        class IMaterialModel : public core::Model
        {
            using super = core::Model;

        public:
            IMaterialModel(const core::string & _name, core::IObject * _parent) :
                super(_name, _parent)
            {

            }

            virtual core::uint              GetTextureCount () const = 0;
            virtual driver::ITexture *      GetTexture      (core::uint _index) const = 0;
            virtual const core::string &    GetTexturePath  (core::uint _index) const = 0;

            virtual void                    SetTexture      (core::uint _index, driver::ITexture * _texture) = 0;
        };
    }
}