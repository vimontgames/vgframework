#pragma once

#include "core/Object/Object.h"
#include "core/Pool/Pool.h"
#include "gfx/FrameGraph/FrameGraph.h"

namespace vg::gfx
{
	class SubPass;

	namespace base
	{
		class RenderPass : public core::Object
		{
		public:
			RenderPass(const RenderPassKey & _key);
			virtual ~RenderPass();

            void addSubPass(gfx::SubPass * _subPass);
            const core::vector<gfx::SubPass*> & getSubPasses() const;

            const RenderPassKey & getRenderPassKey() const { return m_renderPassKey; }
			
		//private:
            RenderPassKey                               m_renderPassKey;
			core::vector<gfx::SubPass *>		        m_subPasses;
			core::vector<FrameGraph::TextureResource *> m_colorAttachments;
            FrameGraph::TextureResource *               m_depthStencilAttachment;
		};
	}
}

#include VG_GFXAPI_HEADER(RenderPass)

namespace vg::gfx
{
	class RenderPass : public VG_GFXAPI::RenderPass
	{
		using super = VG_GFXAPI::RenderPass;

	public:
        const char * getClassName() const final { return "RenderPass"; }

		RenderPass(const RenderPassKey & _key);
		~RenderPass();

		void finalize();

    private:
        static core::Pool<RenderPass> s_memPool;
	};
}