#pragma once

#include "core/Model/Model.h"

namespace vg
{
    namespace gfx
    {
        class ITexture;
    }

    namespace graphics::renderer
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
            virtual gfx::ITexture *      GetTexture      (MaterialTextureType _type) const = 0;
            virtual const core::string &    GetTexturePath  (MaterialTextureType _type) const = 0;

            virtual void                    SetTexture      (MaterialTextureType _type, gfx::ITexture * _texture) = 0;
        };
    }
}