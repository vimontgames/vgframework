#pragma once

#include "gfx/FrameGraph/SubPass.h"

namespace vg::gfx::dx12
{
	class SubPass : public base::SubPass
	{
		using super = base::SubPass;

	public:
		SubPass();
		~SubPass();

	//private:
		D3D12_RENDER_PASS_FLAGS					m_d3d12renderPassFlags = D3D12_RENDER_PASS_FLAG_NONE;
		D3D12_RENDER_PASS_RENDER_TARGET_DESC	m_d3d12renderPassRenderTargetDesc[maxRenderTarget];
		D3D12_RENDER_PASS_DEPTH_STENCIL_DESC	m_d3d12renderPassDepthStencilDesc;
		core::u8								m_renderTargetCount : 7;
		core::u8								m_depthStencilCount : 1;
	};
}