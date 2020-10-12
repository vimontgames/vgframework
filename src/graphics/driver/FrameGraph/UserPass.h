#pragma once

#include "core/Object/Object.h"
#include "graphics/driver/FrameGraph/FrameGraph.h"
#include "graphics/driver/Shader/ShaderKey.h"

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
        FrameGraph::TextureResource * getDepthStencil() const;

        core::uint getRenderTargetCount() const;
        const FrameGraph::TextureResourceDesc & getRenderTargetDesc(core::uint _index) const;

        bool hasDepthStencil() const;
        const FrameGraph::TextureResourceDesc &  getDepthStencilDesc() const;

	protected:
        void declareRenderTarget(const FrameGraph::ResourceID & _resID, const FrameGraph::TextureResourceDesc & _resDesc);
		void writeRenderTarget(core::uint _slot, const FrameGraph::ResourceID & _resID);

	private:
		friend class FrameGraph;

		void setFrameGraph(FrameGraph * _frameGraph);

	protected:
		FrameGraph *								m_frameGraph = nullptr;

		FrameGraph::TextureResource *				m_depthStencil;
		core::vector<FrameGraph::TextureResource *>	m_renderTarget;
	};
}