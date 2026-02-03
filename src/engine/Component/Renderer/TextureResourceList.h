#pragma once

#include "core/Resource/ResourceList.h"
#include "engine/Resource/Texture/TextureResource.h"

namespace vg::engine
{
    class TextureResourceList : public core::ResourceList<TextureResource>
    {
    public:
        VG_CLASS_DECL(TextureResourceList, ResourceList<TextureResource>);

        TextureResourceList(const core::string & _name, core::IObject * _parent);
        ~TextureResourceList();
    };
}