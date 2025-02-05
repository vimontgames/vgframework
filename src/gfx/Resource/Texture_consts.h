#pragma once

#include "Resource_consts.h"

namespace vg::gfx
{
	vg_enum_class(vg::gfx, TextureType, core::u8,
		Texture1D			= 0x01,
		Texture2D			= 0x02,
		Texture3D			= 0x03,
		TextureCube			= 0x06,
		Texture1DArray		= 0x11,
		Texture2DArray		= 0x12,
		TextureCubeArray	= 0x16,
		Texture2DMS			= 0x22,
		Texture2DMSArray	= 0x31
	);

	vg_enum_class(vg::gfx, PixelFormat, core::u8,
		Unknow = 0,		
		R8G8B8A8_unorm,
		R8G8B8A8_unorm_sRGB,
		B8G8R8A8_unorm,
		B8G8R8A8_unorm_sRGB,
		R10G10B10A2_unorm,
		R16G16B16A16_unorm,
		R16G16B16A16_float,
		R16G16B16A16_uint,
		R16G16_float,
		R32G32B32A32_float,
		R32_uint,
		R32G32_uint,
		D32S8
	);

	vg_enum_class(vg::gfx, TextureFlags, core::u32,
		None			= 0x00000000,
        Backbuffer		= 0x00000001,
		RenderTarget	= 0x00000002,
        DepthStencil    = 0x00000004
	);

	vg_enum_class(vg::gfx, MSAA, core::u8,
        None    = 0,
        MSAA2X  = 1,
        MSAA4X  = 2,
        MSAA8X  = 3,
        MSAA16X = 4
    );

    vg_enum_class(vg::gfx, MSAAFlags, core::u8,
        None	= 1 << (core::u8)MSAA::None,
        MSAA2X	= 1 << (core::u8)MSAA::MSAA2X, 
        MSAA4X	= 1 << (core::u8)MSAA::MSAA4X, 
        MSAA8X	= 1 << (core::u8)MSAA::MSAA8X, 
        MSAA16X = 1 << (core::u8)MSAA::MSAA16X
    );

	class TextureDesc
	{
	public:
		TextureDesc(Usage _usage = Usage::Default, BindFlags _bindFlags = BindFlags::ShaderResource, CPUAccessFlags _cpuAccessFlags = CPUAccessFlags::None, TextureType _type = TextureType::Texture2D, PixelFormat _format = PixelFormat::R8G8B8A8_unorm, TextureFlags _flags = TextureFlags::None, core::u16 _width = 1, core::u16 _height = 1, core::u16 _slices = 1, core::u8 _mipmaps = 1, MSAA _msaa = MSAA::None) :
            resource(_usage, _bindFlags, _cpuAccessFlags),
            width(_width),
			height(_height),
			slices(_slices),
			mipmaps(_mipmaps),
			type(_type),
			format(_format),
			flags(_flags),
			msaa(_msaa)
		{

		}

        ResourceDesc    resource;
		core::u16		width;
		core::u16		height;
		core::u16		slices;
		core::u8		mipmaps;
		TextureType		type;
		PixelFormat		format;
		TextureFlags	flags;
		MSAA			msaa;

		VG_INLINE bool testTextureFlags		(TextureFlags _flags) const { return core::asBool(core::operator & (_flags,flags)); }

        VG_INLINE bool isBackbuffer			() const { return testTextureFlags(TextureFlags::Backbuffer); }
        VG_INLINE bool isRenderTarget		() const { return testTextureFlags(TextureFlags::RenderTarget); }
        VG_INLINE bool isDepthStencil		() const { return testTextureFlags(TextureFlags::DepthStencil); }

		VG_INLINE bool isShaderResource		() const { return resource.testBindFlags(BindFlags::ShaderResource); }

		VG_INLINE bool operator != (const TextureDesc & _other) const { return resource != _other.resource || width != _other.width || height != _other.height || slices != _other.slices || mipmaps != _other.mipmaps || type != _other.type || format != _other.format || flags != _other.flags; }
        VG_INLINE bool operator == (const TextureDesc & _other) const { return !(_other != *this); }

        VG_INLINE static core::uint getMSAASampleCount(MSAA _msaa) { return 1 << (core::uint)_msaa; }
	};
}