#pragma once

#include "core/Object/Object.h"
#include "graphics/driver/FrameGraph/FrameGraph.h"

namespace vg::graphics::driver
{
	class CommandList;
    class Shader;

	//--------------------------------------------------------------------------------------
	// An API-agnostic Render/Compute pass exposed to the client Renderer
	// It will used to generated the required RenderPasses/SubPasses by the FrameGraph
	//--------------------------------------------------------------------------------------
	class UserPass : public core::Object
	{
	public:
						UserPass	();
		virtual			~UserPass	();

		void			reset		();

		virtual void	setup		() = 0;
		virtual void	draw		(CommandList * _cmdList) const = 0;

		const core::vector<FrameGraph::TextureResource *> & getRenderTargets() const;

	protected:
		void writeRenderTarget(core::uint _slot, const FrameGraph::ResourceID & _resID, const FrameGraph::TextureDesc & _resDesc);

	private:
		friend class FrameGraph;

		void setFrameGraph(FrameGraph * _frameGraph);

	protected:
		FrameGraph *								m_frameGraph = nullptr;

		FrameGraph::TextureResource *				m_depthStencilIn;
		FrameGraph::TextureResource *				m_depthStencilOut;

		core::vector<FrameGraph::TextureResource *>	m_renderTargetIn;
		core::vector<FrameGraph::TextureResource *>	m_renderTargetOut;
	};
}