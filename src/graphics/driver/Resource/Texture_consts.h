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
        R8G8B8A8_unorm_sRGB,

        B8G8R8A8_unorm,
        B8G8R8A8_unorm_sRGB,

        D32S8
	};

	enum class TextureFlags : core::u32
	{
		None			= 0x00000000,
        Backbuffer		= 0x00000001,
		RenderTarget	= 0x00000002,
        DepthStencil    = 0x00000004
	};

	class TextureDesc
	{
	public:
		TextureDesc(Usage _usage = Usage::Default, BindFlags _bindFlags = BindFlags::ShaderResource, CPUAccessFlags _cpuAccessFlags = CPUAccessFlags::None, TextureType _type = TextureType::Texture2D, PixelFormat _format = PixelFormat::R8G8B8A8_unorm, TextureFlags _flags = TextureFlags::None, core::u16 _width = 1, core::u16 _height = 1, core::u16 _depth = 1, core::u8 _mipmaps = 1) :
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

        inline bool testFlags(TextureFlags _flags) const { return 0 != (std::underlying_type<TextureFlags>::type(_flags) & std::underlying_type<TextureFlags>::type(flags)); }

        inline bool isBackbuffer        () const { return testFlags(TextureFlags::Backbuffer); }
        inline bool isRenderTarget      () const { return testFlags(TextureFlags::RenderTarget); }
        inline bool isDepthStencil      () const { return testFlags(TextureFlags::DepthStencil); }

        inline bool isShaderResource    () const { return resource.isShaderResource(); }
	};
}