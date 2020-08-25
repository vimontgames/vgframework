#pragma once

#include "graphics/driver/Texture/Texture.h"

namespace vg::graphics::driver::dx12
{
	class Texture : public base::Texture
	{
		using super = base::Texture;

	public:
											Texture				(const TextureDesc & _texDesc, const core::string & _name, void * _initData);
											~Texture			();

		void								setd3d12Resource	(ID3D12Resource * _resource);
		ID3D12Resource *					getd3d12Resource	() const;
		const D3D12_CPU_DESCRIPTOR_HANDLE & getd3d12RTVHandle	() const;

	private:
		static const inline D3D12_CPU_DESCRIPTOR_HANDLE s_d3d12invalidRenderTargetViewHandle = { (size_t)-1 };

		ID3D12Resource *					m_d3d12resource = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE			m_d3d12RTVHandle = s_d3d12invalidRenderTargetViewHandle;
	};
}