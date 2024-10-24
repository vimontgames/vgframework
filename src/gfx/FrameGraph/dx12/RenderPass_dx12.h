#pragma once

#include "gfx/FrameGraph/RenderPass.h"

namespace vg::gfx::dx12
{
	class RenderPass : public base::RenderPass
	{
		using super = base::RenderPass;

	public:
		RenderPass(RenderPassType _renderPassType, const RenderPassKey & _key);
		~RenderPass();

		void finalize();
	};
}