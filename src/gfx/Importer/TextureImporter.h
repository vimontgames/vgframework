#pragma once

#include "Importer.h"
#include "core/Singleton/Singleton.h"
#include "gfx/Resource/Texture_consts.h"

namespace vg::gfx
{
    class TextureDesc;
    struct TextureImporterSettings;

    enum class TextureImporterType : core::u8;
    enum class TextureImporterFormat : core::u8;

    class TextureImporter : public Importer, public core::Singleton<TextureImporter>
    {
    public:
        bool importTextureData(const core::string & _path, TextureDesc & _desc, core::vector<core::u8> & _buffer, const TextureImporterSettings * _importSettings);

    private:
        core::u32 computeMaxMipmapCount(TextureImporterType _type, core::uint _width, core::uint _height, core::uint _depth);
        bool generateMipmaps(const core::u8 * _src, const TextureDesc & _desc, core::vector<core::u8> & _buffer);
    };
}
