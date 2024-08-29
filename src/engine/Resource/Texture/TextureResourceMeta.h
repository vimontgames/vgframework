#pragma once

#include "core/Resource/ResourceMeta.h"
#include "renderer/TextureImportSettings.h"

namespace vg::engine
{
    class TextureResourceMeta : public core::ResourceMeta
    {
    public:
        VG_CLASS_DECL(TextureResourceMeta, core::ResourceMeta)

        TextureResourceMeta(const core::string & _name = "", core::IObject * _parent = nullptr);
        ~TextureResourceMeta();

        const renderer::TextureImportSettings & getTextureImportSettings() const { return m_importSettings; }

    private:
        renderer::TextureImportSettings m_importSettings;
    };
}