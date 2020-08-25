#pragma once

#include "core/IPlugin.h"

namespace vg::graphics::driver
{
	struct DeviceParams;
}

namespace vg::graphics::renderer
{
	struct RendererParams
	{
		driver::DeviceParams & device;
	};

	class IRenderer : public core::IPlugin
	{
	public:
		virtual void	init			(const RendererParams & _params) = 0;
		virtual void	deinit			() = 0;

		virtual void	runOneFrame		() = 0;
	};
}