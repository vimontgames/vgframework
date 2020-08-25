#pragma once

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
		R8G8B8A8 = 0
	};

	enum class TextureFlags : core::u32
	{
		None			= 0x00000000,
		ShaderResource	= 0x00000001,
		RenderTarget	= 0x00000002,			
		Backbuffer		= 0x00000010,
		sRGB			= 0x00000020
	};

	class TextureDesc
	{
	public:
		TextureDesc(TextureType _type, PixelFormat _format, TextureFlags _flags, core::u16 _width, core::u16 _height, core::u16 _depth = 1, core::u8 _mipmaps = 1) :
			width(_width),
			height(_height),
			depth(_depth),
			mipmaps(_mipmaps),
			type(_type),
			format(_format),
			flags(_flags)
		{

		}

		core::u16		width;
		core::u16		height;
		core::u16		depth;
		core::u8		mipmaps;
		TextureType		type;
		PixelFormat		format;
		TextureFlags	flags;
	};
}