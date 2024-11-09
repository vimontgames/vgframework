#pragma once

#include "Importer.h"
#include "core/Singleton/Singleton.h"
#include "gfx/Resource/Texture_consts.h"
#include "gfx/Importer/TextureImporterSettings.h"

namespace vg::gfx
{
    class TextureDesc;
    struct TextureImporterSettings;

    enum class ComponentFormat
    {
        UByte = 0,
        UShort,
        UInt,
        Half,
        Float
    };

    // TODO: move to TextureImporterSettings.h ?
    template <TextureImporterFormat FMT> struct TextureImporterFormatTraits;
    template <> struct TextureImporterFormatTraits<TextureImporterFormat::RGBA8> { using type = core::u8; static const core::uint components = 4; };
    template <> struct TextureImporterFormatTraits<TextureImporterFormat::RGBA32f> { using type = float; static const core::uint components = 4; };

    class TextureImporter : public Importer, public core::Singleton<TextureImporter>
    {
    public:
        bool importTextureData(const core::string & _path, TextureDesc & _desc, core::vector<core::u8> & _finalBuffer, const TextureImporterSettings * _importSettings);

    private:
        core::u32 computeMaxMipmapCount(TextureImporterType _type, core::uint _width, core::uint _height, core::uint _depth);
        template <TextureImporterFormat SrcFmt> bool generateMipmaps(const typename TextureImporterFormatTraits<SrcFmt>::type * _src, const TextureDesc & _desc, core::vector<typename TextureImporterFormatTraits<SrcFmt>::type> & _buffer);
        template <TextureImporterFormat SrcFmt, TextureImporterFormat DstFmt> bool convert(const TextureDesc & _desc, const core::vector<typename TextureImporterFormatTraits<SrcFmt>::type> & _in, core::vector<core::u8> & _out);
    };
}
