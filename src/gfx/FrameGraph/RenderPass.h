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
			RenderPass(RenderPassType _renderPassType, const RenderPassKey & _key);
			virtual ~RenderPass();

			RenderPassType getRenderPassType() const;

            void addSubPass(gfx::SubPass * _subPass);
            const core::vector<gfx::SubPass*> & getSubPasses() const;

			const core::vector<FrameGraphTextureResource *> & getColorAttachments() const;
			const FrameGraphTextureResource * getDepthStencilAttachment() const;

            const RenderPassKey & getRenderPassKey() const { return m_renderPassKey; }
			
		//protected:
			const RenderPassType						m_renderPassType;
            const RenderPassKey							m_renderPassKey;
			core::vector<gfx::SubPass *>		        m_subPasses;
			core::vector<FrameGraphTextureResource *>	m_colorAttachments;
            FrameGraphTextureResource *					m_depthStencilAttachment;
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
        const char * GetClassName() const final { return "RenderPass"; }

		RenderPass(RenderPassType _renderPassType, const RenderPassKey & _key);
		~RenderPass();

		void finalize();

    private:
        static core::Pool<RenderPass> s_memPool;
	};
}