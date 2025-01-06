#pragma once

#include "core/Object/Object.h"
#include "gfx/FrameGraph/FrameGraph.h"
#include "gfx/Shader/ShaderKey.h"
#include "gfx/Shader/ComputeShaderKey.h"
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
							UserPass			(const core::string & _name);
		virtual				~UserPass			();

		void				reset				();

		// Called when building FrameGraph, no access to CommandList
		virtual void		Setup				(const RenderPassContext & _renderPassContext) = 0;

        // An heuristic used during job dispatch where 1 ~= 1 draw call. It can be a fixed cost and/or variable cost
        virtual core::u64	GetCostEstimate		(const RenderPassContext & _renderPassContext) const { return 1; }

		// Called on main thread for all nodes before rendering using multiple threads (e.g., compute offsets used in other RenderJobs)
		virtual void		BeforeAll			(const RenderPassContext & _renderPassContext) {};

		// Called before entering RenderPass (e.g., write buffers from CPU to the GPU)
		virtual void		BeforeRender		(const RenderPassContext & _renderPassContext, CommandList * _cmdList) {}

		// Called after entering RenderPass for drawing stuff
		virtual void		Render				(const RenderPassContext & _renderPassContext, CommandList * _cmdList) const = 0;

        // Called after exiting RenderPass (e.g., read buffer from GPU to the CPU)
        virtual void		AfterRender			(const RenderPassContext & _renderPassContext, CommandList * _cmdList) {};

        // Called on main thread for all nodes after rendering using multiple threads (e.g., process GPU readback data)
        virtual void		AfterAll			(const RenderPassContext & _renderPassContext) {};

		// Descriptor for framegraph texture/buffer
		const FrameGraphTextureResourceDesc * getTextureResourceDesc(const FrameGraphResourceID & _resID) const;
		const FrameGraphBufferResourceDesc * getBufferResourceDesc(const FrameGraphResourceID & _resID) const;

		// Textures being bound as color buffers
		VG_INLINE const core::vector<FrameGraphTextureResource *> & getRenderTargets() const;

		// Texture being bound as depth/stencil buffer
		VG_INLINE FrameGraphTextureResource * getDepthStencil() const;

		// RWTextures being accessed during the pass
        VG_INLINE const core::vector<FrameGraphTextureResource *> & getRWTextures() const;

		// RWBuffers being accessed during the pass 
		VG_INLINE const core::vector<FrameGraphBufferResource *> & getRWBuffers() const;

        core::uint getRenderTargetCount() const;
        const FrameGraphTextureResourceDesc & getRenderTargetDesc(core::uint _index) const;

        bool hasDepthStencil() const;
        const FrameGraphTextureResourceDesc &  getDepthStencilDesc() const;

        const core::vector<FrameGraphTextureResource *> & getTexturesRead() const { return m_textures; }

		void setUserPassType(RenderPassType _userPassType);
		RenderPassType getUserPassType() const;

	protected:
        // setup
        void createRenderTarget(const FrameGraphResourceID & _resID, FrameGraphTextureResourceDesc & _resDesc);
		void writeRenderTarget(core::uint _slot, const FrameGraphResourceID & _resID);
        void readRenderTarget(const FrameGraphResourceID & _resID);

        void createDepthStencil(const FrameGraphResourceID & _resID, FrameGraphTextureResourceDesc & _resDesc);
        void writeDepthStencil(const FrameGraphResourceID & _resID);
        void readDepthStencil(const FrameGraphResourceID & _resID);
		void readDepthStencil(const core::vector<FrameGraphResourceID> & _resIDs);

		void createRWTexture(const FrameGraphResourceID & _resID, FrameGraphTextureResourceDesc & _resDesc);
        void writeRWTexture(const FrameGraphResourceID & _resID);
        void readRWTexture(const FrameGraphResourceID & _resID);

		void createRWBuffer(const FrameGraphResourceID & _resID, FrameGraphBufferResourceDesc & _resDesc);
		void writeRWBuffer(const FrameGraphResourceID & _resID);
		void readRWBuffer(const FrameGraphResourceID & _resID);

        // draw
        Texture * getRenderTarget(const FrameGraphResourceID & _resID) const;
		Texture * getDepthStencil(const FrameGraphResourceID & _resID) const;
		Texture * getRWTexture(const FrameGraphResourceID & _resID) const;
		Buffer * getRWBuffer(const FrameGraphResourceID & _resID) const;

	private:
		friend class FrameGraph;

		void setFrameGraph(FrameGraph * _frameGraph);
		FrameGraphTextureResource * createTexture(const FrameGraphResourceID & _resID, FrameGraphTextureResourceDesc & _resDesc);
		FrameGraphBufferResource * createBuffer(const FrameGraphResourceID & _resID, FrameGraphBufferResourceDesc & _resDesc);

	protected:
		FrameGraph *								m_frameGraph = nullptr;
		RenderPassType								m_userPassType = (RenderPassType)-1;

        // Inputs
        core::vector<FrameGraphTextureResource *>	m_textures;

        // Outputs
		FrameGraphTextureResource *					m_depthStencil;
		core::vector<FrameGraphTextureResource *>	m_renderTargets;

		// Input and/or output
		core::vector<FrameGraphTextureResource *>	m_rwTextures;
		core::vector<FrameGraphBufferResource *>	m_rwBuffers;
	};
}

#if VG_ENABLE_INLINE
#include "UserPass.inl"
#endif