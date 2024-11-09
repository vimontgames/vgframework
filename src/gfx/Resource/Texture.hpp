#include VG_GFXAPI_IMPL(Texture)

#include "core/Math/Math.h"
#include "core/Object/EnumHelper.h"

#if !VG_ENABLE_INLINE
#include "Texture.inl"
#endif

namespace vg::gfx
{
	namespace base
	{
        //--------------------------------------------------------------------------------------
        core::u16 Texture::GetWidth() const
        {
            return m_texDesc.width;
        }

        //--------------------------------------------------------------------------------------
        core::u16 Texture::GetHeight() const
        {
            return m_texDesc.height;
        }

        //--------------------------------------------------------------------------------------
        gfx::PixelFormat Texture::GetPixelFormat() const
        {
            return m_texDesc.format;
        }

        //--------------------------------------------------------------------------------------
        TextureType Texture::GetTextureType() const
        {
            return m_texDesc.type;
        }

        //--------------------------------------------------------------------------------------
        core::u16 Texture::computeMaxMip(core::uint _dim)
        {
            auto count = log2((float)_dim)+1;
            return (core::uint)count;
        }

        //--------------------------------------------------------------------------------------
        core::u32 Texture::computeMaxMipmapCount(const TextureDesc & _texDesc)
        {
            switch (_texDesc.type)
            {
                case TextureType::Texture1D:
                case TextureType::Texture1DArray:
                    return computeMaxMip(_texDesc.width);

                case TextureType::Texture2D:
                case TextureType::TextureCube:
                case TextureType::Texture2DArray:
                case TextureType::TextureCubeArray:
                    return computeMaxMip(min(_texDesc.width, _texDesc.height));

                case TextureType::Texture3D:
                    return computeMaxMip(min(min(_texDesc.width, _texDesc.height), _texDesc.slices));

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
                    return -1;
                case PixelFormat::R8G8B8A8_unorm:
                case PixelFormat::R8G8B8A8_unorm_sRGB:
                    return sizeof(u8) * 4;

                case PixelFormat::R16G16B16A16_unorm:
                    return sizeof(u16) * 4;

                case PixelFormat::R16G16_float:
                    return sizeof(u16) * 2;

                case PixelFormat::R32G32B32A32_float:
                    return sizeof(float) * 4;
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

            // explicit initialization is needed because of union
            m_depthTextureHandle = BindlessTextureHandle();
            m_stencilTextureHandle = BindlessTextureHandle();
		}

		//--------------------------------------------------------------------------------------
		Texture::~Texture()
		{

		}
	}

	//--------------------------------------------------------------------------------------
	// Texture
	//--------------------------------------------------------------------------------------
    VG_REGISTER_OBJECT_CLASS_EX(Texture, "Texture", ClassDescFlags::Model);

    //--------------------------------------------------------------------------------------
    bool Texture::registerProperties(IClassDesc & _desc)
    {
        registerPropertyEnumEx(Texture, PixelFormat, m_texDesc.format, "Format", PropertyFlags::ReadOnly);
        
        registerPropertyEx(Texture, m_texDesc.width, "Width", PropertyFlags::ReadOnly);
        setPropertyDescription(Texture, m_texDesc.width, "Texture width");

        registerPropertyEx(Texture, m_texDesc.height, "Height", PropertyFlags::ReadOnly);
        setPropertyDescription(Texture, m_texDesc.height, "Texture height");

        return true;
    }

    //--------------------------------------------------------------------------------------
    Texture::Texture(const TextureDesc & _texDesc, const core::string & _name, const void * _initData, ReservedSlot _reservedSlot) :
		super::Texture(_texDesc, _name, _initData, _reservedSlot)
	{
        SetName(_name);
	}

	//--------------------------------------------------------------------------------------
	Texture::~Texture()
	{
        auto * device = Device::get();
        auto * bindlessTable = device->getBindlessTable();

        if (isDepthStencilFormat(getTexDesc().format))
        {
            if (m_depthTextureHandle.isValid())
                bindlessTable->freeBindlessTextureHandle(m_depthTextureHandle);

            if (m_stencilTextureHandle.isValid())
                bindlessTable->freeBindlessTextureHandle(m_stencilTextureHandle);
        }
        else
        {
            if (m_textureHandle.isValid())
                bindlessTable->freeBindlessTextureHandle(m_textureHandle);
        }       

        if (m_rwTextureHandle.isValid())
            bindlessTable->freeBindlessRWTextureHandle(m_rwTextureHandle);
	}
}