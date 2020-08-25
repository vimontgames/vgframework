#pragma once

#include "engine/IEngine.h"

namespace vg::engine
{
	class Engine : public IEngine
	{
	public:
		IPlugin::Version				getVersion	() const override;

										Engine		();
										~Engine		();

		void							init		(const EngineParams & _params) override;
		void							deinit		() override;

		void							runOneFrame	() override;

		graphics::renderer::IRenderer *	getRenderer	() const override;

	private:
		graphics::renderer::IRenderer *	m_renderer = nullptr;
	};
}