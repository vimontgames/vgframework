#pragma once

#include "Importer.h"
#include "core/Singleton/Singleton.h"
#include "gfx/Resource/Texture_consts.h"
#include "core/Types/Traits.h"

namespace vg::gfx
{
    enum class TextureImporterType : core::u8;
}

namespace vg::core
{
    struct UByte4
    {
        core::u8 r, g, b, a;
    };
    template <> struct vectorTraits<UByte4> { using type = core::u8; static const uint count = 4; };

    struct UShort4
    {
        core::u16 r, g, b, a;
    };
    template <> struct vectorTraits<UShort4> { using type = core::u16; static const uint count = 4; };
}

namespace vg::gfx
{
    class TextureDesc;
    struct TextureImporterSettings;

    class TextureImporter : public Importer, public core::Singleton<TextureImporter>
    {
    public:
        bool importTextureData(const core::string & _path, TextureDesc & _desc, core::vector<core::u8> & _finalBuffer, const TextureImporterSettings * _importSettings);

    private:
        core::u32 computeMaxMipmapCount(TextureImporterType _type, core::uint _width, core::uint _height, core::uint _depth);
        template <typename SourceFormat> bool generateMipmaps(const SourceFormat * _src, const TextureDesc & _desc, core::vector<SourceFormat> & _buffer);
        template <typename SourceFormat, typename DestFormat> bool convert(const TextureDesc & _desc, const core::vector<SourceFormat> & _in, core::vector<core::u8> & _out);
    };
}
