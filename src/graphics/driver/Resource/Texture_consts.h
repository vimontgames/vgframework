#pragma once

#include "Resource_consts.h"

namespace vg::graphics::driver
{
	enum class TextureType : core::u8
	{
		Texture1D			= 0x01,
		Texture2D			= 0x02,
		Texture3D			= 0x03,
		TextureCube			= 0x06,
		Texture1DArray		= 0x11,
		Texture2DArray		= 0x12,
		TextureCubeArray	= 0x16,
		Texture2DMS			= 0x22,
		Texture2DMSArray	= 0x31
	};

	enum class PixelFormat : core::u8
	{
        Unknow = 0,
		R8G8B8A8_unorm,
        B8G8R8A8_unorm
	};

	enum class TextureFlags : core::u32
	{
		None			= 0x00000000,
        Backbuffer		= 0x00000001,
		RenderTarget	= 0x00000002,			
		sRGB			= 0x00000004
	};

	class TextureDesc
	{
	public:
		TextureDesc(Usage _usage, BindFlags _bindFlags, CPUAccessFlags _cpuAccessFlags, TextureType _type, PixelFormat _format, TextureFlags _flags, core::u16 _width, core::u16 _height, core::u16 _depth = 1, core::u8 _mipmaps = 1) :
            resource(_usage, _bindFlags, _cpuAccessFlags),
            width(_width),
			height(_height),
			depth(_depth),
			mipmaps(_mipmaps),
			type(_type),
			format(_format),
			flags(_flags)
		{

		}

        ResourceDesc    resource;
		core::u16		width;
		core::u16		height;
		core::u16		depth;
		core::u8		mipmaps;
		TextureType		type;
		PixelFormat		format;
		TextureFlags	flags;
	};
}