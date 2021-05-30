#include VG_GFXAPI_IMPL(Texture)

#include "core/Math/Math.h"

namespace vg::graphics::driver
{
	namespace base
	{
        //--------------------------------------------------------------------------------------
        core::u32 Texture::computeMaxMipmapCount(const TextureDesc & _texDesc)
        {
            switch (_texDesc.type)
            {
                case TextureType::Texture1D:
                case TextureType::Texture1DArray:
                    return (core::u32)(1 + floor(log10((float)_texDesc.width) / log10(2.0)));

                case TextureType::Texture2D:
                case TextureType::TextureCube:
                case TextureType::Texture2DArray:
                case TextureType::TextureCubeArray:
                    return (core::u32)(1 + floor(log10((float)max(_texDesc.width, _texDesc.height)) / log10(2.0)));

                case TextureType::Texture3D:
                    return (core::u32)(1 + floor(log10((float)max( max(_texDesc.width, _texDesc.height), _texDesc.depth)) / log10(2.0)));

                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(_texDesc.type);

                case TextureType::Texture2DMS:
                case TextureType::Texture2DMSArray:
                    return 1;
            }            
        }

        //--------------------------------------------------------------------------------------
        core::u32 Texture::getPixelFormatSize(PixelFormat _format)
        {
            switch (_format)
            {
                default:
                    VG_ASSERT(false, "Unhandled PixelFormat \"%s\" (%u)", asString(_format).c_str(), _format);
                case PixelFormat::R8G8B8A8_unorm:
                case PixelFormat::R8G8B8A8_unorm_sRGB:
                    return 4;
            }
        }

        //--------------------------------------------------------------------------------------
        bool Texture::isDepthStencilFormat(PixelFormat _format)
        {
            switch (_format)
            {
                default:
                    VG_ASSERT(false, "Unhandled PixelFormat \"%s\" (%u)", asString(_format).c_str(), _format);
                case PixelFormat::R8G8B8A8_unorm:
                case PixelFormat::R8G8B8A8_unorm_sRGB:
                case PixelFormat::B8G8R8A8_unorm:
                case PixelFormat::B8G8R8A8_unorm_sRGB:
                case PixelFormat::R16G16B16A16_float:
                    return false;

                case PixelFormat::D32S8:
                    return true;
            }
        }

		//--------------------------------------------------------------------------------------
		Texture::Texture(const TextureDesc & _texDesc, const core::string & _name, const void * _initData) :
            super(_name, nullptr),
			m_texDesc(_texDesc)
		{
            //VG_DEBUGPRINT("[Device] Init %s \"%s\" (", asString(_texDesc.type).c_str(), _name.c_str());
            //VG_DEBUGPRINT("%u", _texDesc.width);
            //VG_DEBUGPRINT("x%u", _texDesc.height);
            //if (_texDesc.depth > 1)
            //	VG_DEBUGPRINT("x%u", _texDesc.depth);
            //VG_DEBUGPRINT(" %s", asString(_texDesc.format).c_str());
            //
            //bool first = true;
            //for (uint i = 0; i < countBits(_texDesc.flags); ++i)
            //{
            //	const TextureFlags flag = TextureFlags(1 << i);
            //	if (asBool(_texDesc.flags & flag))
            //	{
            //		VG_DEBUGPRINT("%c%s", first ? ' ':'|', asString(flag).c_str());
            //		first = false;
            //	}
            //}
            //VG_DEBUGPRINT(")\n");
		}

		//--------------------------------------------------------------------------------------
		Texture::~Texture()
		{

		}
	}

	//--------------------------------------------------------------------------------------
	// Texture
	//--------------------------------------------------------------------------------------
	Texture::Texture(const TextureDesc & _texDesc, const core::string & _name, const void * _initData, ReservedSlot _reservedSlot) :
		super::Texture(_texDesc, _name, _initData, _reservedSlot)
	{

	}

	//--------------------------------------------------------------------------------------
	Texture::~Texture()
	{
        auto * device = Device::get();
        device->getBindlessTable()->freeBindlessTextureHandle(m_bindlessSRVHandle);
	}
}