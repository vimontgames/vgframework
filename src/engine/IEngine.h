#pragma once

#include "core/IPlugin.h"

namespace vg::graphics::renderer
{
	struct RendererParams;
	class IRenderer;
}

namespace vg::engine
{
	struct EngineParams
	{
		vg::graphics::renderer::RendererParams & renderer;
	};

	class IEngine : public core::IPlugin
	{
	public:
		virtual void							init		(const EngineParams & _params) = 0;
		virtual void							deinit		() = 0;

		virtual void							runOneFrame	() = 0;

		virtual graphics::renderer::IRenderer *	getRenderer	() const = 0;
	};
}