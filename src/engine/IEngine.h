#pragma once

#include "core/IPlugin.h"
#include "Engine_Consts.h"

namespace vg::graphics::renderer
{
	class IRenderer;
}

namespace vg::engine
{
	class IEngine : public core::IPlugin
	{
	public:
		virtual void							init		(const EngineParams & _params) = 0;
		virtual void							deinit		() = 0;

		virtual void							runOneFrame	() = 0;

		virtual graphics::renderer::IRenderer *	getRenderer	() const = 0;
	};
}