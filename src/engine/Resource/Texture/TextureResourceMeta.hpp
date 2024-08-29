#include "TextureResourceMeta.h"
#include "gfx/Resource/Texture_consts.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_RESOURCE_CLASS(TextureResourceMeta, "TextureResourceMeta");

    //--------------------------------------------------------------------------------------
    bool TextureResourceMeta::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerOptionalPropertyEnum(TextureResourceMeta, m_importSettings.m_overridePixelFormat, gfx::PixelFormat, m_importSettings.m_pixelFormat, "Format");
        registerOptionalPropertyEnum(TextureResourceMeta, m_importSettings.m_overrideDownscale, renderer::Downscale, m_importSettings.m_downscale, "Downscale");

        return true;
    }

    //--------------------------------------------------------------------------------------
    TextureResourceMeta::TextureResourceMeta(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    TextureResourceMeta::~TextureResourceMeta()
    {

    }
}