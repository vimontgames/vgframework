#include "TextureResourceMeta.h"
#include "gfx/Importer/TextureImporterSettings.h"
#include "core/string/string.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS(TextureResourceMeta, "TextureResourceMeta");

    //--------------------------------------------------------------------------------------
    bool TextureResourceMeta::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEnum(TextureResourceMeta, gfx::TextureImporterType, m_importSettings.m_importerType, "Type");
        registerPropertyEnum(TextureResourceMeta, gfx::TextureImporterFormat, m_importSettings.m_importerFormat, "Format");
        registerProperty(TextureResourceMeta, m_importSettings.m_sRGB, "sRGB");
        registerPropertyEnum(TextureResourceMeta, gfx::TextureImporterMip, m_importSettings.m_mipLevelCount, "Mipmaps");
        registerPropertyEnum(TextureResourceMeta, gfx::Downscale, m_importSettings.m_downscale, "Downscale");

        return true;
    }

    //--------------------------------------------------------------------------------------
    TextureResourceMeta::TextureResourceMeta(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {
        string lowerName = tolower(_name);
        if (string::npos != lowerName.find("_albedo") || string::npos != lowerName.find("_diffuse") || string::npos != lowerName.find("_basecolor"))
            this->m_importSettings.m_sRGB = true;
        else
            this->m_importSettings.m_sRGB = false;
    }

    //--------------------------------------------------------------------------------------
    TextureResourceMeta::~TextureResourceMeta()
    {

    }
}