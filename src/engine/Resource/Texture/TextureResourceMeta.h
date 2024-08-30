#pragma once

#include "core/Resource/ResourceMeta.h"
#include "gfx/Importer/TextureImporterSettings.h"

namespace vg::engine
{
    class TextureResourceMeta : public core::ResourceMeta
    {
    public:
        VG_CLASS_DECL(TextureResourceMeta, core::ResourceMeta)

        TextureResourceMeta(const core::string & _name = "", core::IObject * _parent = nullptr);
        ~TextureResourceMeta();

        const gfx::TextureImporterSettings & getTextureImportSettings() const { return m_importSettings; }

    private:
        gfx::TextureImporterSettings m_importSettings;
    };
}