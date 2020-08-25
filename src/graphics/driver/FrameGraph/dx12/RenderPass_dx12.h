#pragma once

#include "graphics/driver/FrameGraph/RenderPass.h"

namespace vg::graphics::driver::dx12
{
	class RenderPass : public base::RenderPass
	{
		using super = base::RenderPass;

	public:
		RenderPass();
		~RenderPass();

		void finalize();
	};
}