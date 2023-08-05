#pragma once

#include "gfx/FrameGraph/SubPass.h"

namespace vg::gfx::vulkan
{
	class SubPass : public base::SubPass
	{
		using super = base::SubPass;

	public:
		SubPass();
		~SubPass();
	};
}