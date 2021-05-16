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
        enum class MaterialTextureType : core::u8;

        class IMaterialModel : public core::Model
        {
            using super = core::Model;

        public:
            IMaterialModel(const core::string & _name, core::IObject * _parent) :
                super(_name, _parent)
            {

            }

            virtual core::uint              GetTextureCount () const = 0;
            virtual driver::ITexture *      GetTexture      (MaterialTextureType _type) const = 0;
            virtual const core::string &    GetTexturePath  (MaterialTextureType _type) const = 0;

            virtual void                    SetTexture      (MaterialTextureType _type, driver::ITexture * _texture) = 0;
        };
    }
}