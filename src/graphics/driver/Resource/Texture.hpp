#include VG_GFXAPI_IMPL(Texture)

namespace vg::graphics::driver
{
	//--------------------------------------------------------------------------------------
	// base::Texture
	//--------------------------------------------------------------------------------------
	namespace base
	{
		//--------------------------------------------------------------------------------------
		Texture::Texture(const TextureDesc & _texDesc, const core::string & _name, void *_initData) :
			m_texDesc(_texDesc)
		{
			VG_DEBUGPRINT("Init %s \"%s\" (", asString(_texDesc.type).c_str(), _name.c_str());
			VG_DEBUGPRINT("%u", _texDesc.width);
			VG_DEBUGPRINT("x%u", _texDesc.height);
			if (_texDesc.depth > 1)
				VG_DEBUGPRINT("x%u", _texDesc.depth);
			VG_DEBUGPRINT(" %s", asString(_texDesc.format).c_str());

			bool first = true;
			for (uint i = 0; i < countBits(_texDesc.flags); ++i)
			{
				const TextureFlags flag = TextureFlags(1 << i);
				if (asBool(_texDesc.flags & flag))
				{
					VG_DEBUGPRINT("%c%s", first ? ' ':'|', asString(flag).c_str());
					first = false;
				}
			}
            int i = 0;
			VG_DEBUGPRINT(")\n");
		}

		//--------------------------------------------------------------------------------------
		Texture::~Texture()
		{

		}
	}

	//--------------------------------------------------------------------------------------
	// Texture
	//--------------------------------------------------------------------------------------
	Texture::Texture(const TextureDesc & _texDesc, const core::string & _name, void * _initData) :
		super::Texture(_texDesc, _name, _initData)
	{

	}

	//--------------------------------------------------------------------------------------
	Texture::~Texture()
	{

	}
}