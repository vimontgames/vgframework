#pragma once

#include "core/Object/Object.h"
#include "gfx/FrameGraph/FrameGraph.h"
#include "gfx/Shader/ShaderKey.h"
#include "gfx/RootSignature/RootSignature_consts.h"
#include "gfx/BindlessTable/BindlessTable_consts.h"

namespace vg::gfx
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
						UserPass	(const core::string & _name);
		virtual			~UserPass	();

		void			reset		();

		virtual void	setup		(const RenderContext & _renderContext, double _dt) = 0;
		virtual void	draw		(const RenderContext & _renderContext, CommandList * _cmdList) const = 0;

		const core::vector<FrameGraph::TextureResource *> & getRenderTargets() const;
        FrameGraph::TextureResource * getDepthStencil() const;

        core::uint getRenderTargetCount() const;
        const FrameGraph::TextureResourceDesc & getRenderTargetDesc(core::uint _index) const;

        bool hasDepthStencil() const;
        const FrameGraph::TextureResourceDesc &  getDepthStencilDesc() const;

        const core::vector<FrameGraph::TextureResource *> & getTexturesRead() const { return m_textures; }

	protected:
        // setup
        void createRenderTarget(const FrameGraph::ResourceID & _resID, FrameGraph::TextureResourceDesc & _resDesc);
		void writeRenderTarget(core::uint _slot, const FrameGraph::ResourceID & _resID);
        void readRenderTarget(const FrameGraph::ResourceID & _resID);

        void createDepthStencil(const FrameGraph::ResourceID & _resID, FrameGraph::TextureResourceDesc & _resDesc);
        void writeDepthStencil(const FrameGraph::ResourceID & _resID);
        void readDepthStencil(const FrameGraph::ResourceID & _resID);

        // draw
        Texture * getRenderTarget(const FrameGraph::ResourceID & _resID) const;

	private:
		friend class FrameGraph;

		void setFrameGraph(FrameGraph * _frameGraph);

	protected:
		FrameGraph *								m_frameGraph = nullptr;

        // Inputs
        core::vector<FrameGraph::TextureResource *> m_textures;

        // Outputs
		FrameGraph::TextureResource *				m_depthStencil;
		core::vector<FrameGraph::TextureResource *>	m_renderTarget;              
	};
}