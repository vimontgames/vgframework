#ifdef VG_DX12
#include "dx12/RenderPass_dx12.hpp"
#elif defined(VG_VULKAN)
#include "vulkan/RenderPass_vulkan.hpp"
#else
#error Undefined GFXAPI
#endif

namespace vg::gfx
{
	namespace base
	{
		//--------------------------------------------------------------------------------------
		RenderPass::RenderPass(RenderPassType _renderPassType, const RenderPassKey & _key) :
			m_renderPassType(_renderPassType),
            m_renderPassKey(_key)
		{

		}

		//--------------------------------------------------------------------------------------
		RenderPass::~RenderPass()
		{
			for (SubPass * subPass : m_subPasses)
				VG_SAFE_RELEASE(subPass);
			m_subPasses.clear();
		}

        //--------------------------------------------------------------------------------------
        RenderPassType RenderPass::getRenderPassType() const
		{
			return m_renderPassType;
		}

		//--------------------------------------------------------------------------------------
		void RenderPass::addSubPass(gfx::SubPass * _subPass)
		{
			m_subPasses.push_back(_subPass);
		}

		//--------------------------------------------------------------------------------------
		const core::vector<gfx::SubPass*> & RenderPass::getSubPasses() const
		{
			return m_subPasses;
		}

        //--------------------------------------------------------------------------------------
        const core::vector<FrameGraphTextureResource *> & RenderPass::getColorAttachments() const
		{
			return m_colorAttachments;
		}

        //--------------------------------------------------------------------------------------
        const FrameGraphTextureResource * RenderPass::getDepthStencilAttachment() const
		{
			return m_depthStencilAttachment;
		}
	}

    core::Pool<RenderPass> RenderPass::s_memPool;

	//--------------------------------------------------------------------------------------
	RenderPass::RenderPass(RenderPassType _renderPassType, const RenderPassKey & _key) :
        super::RenderPass(_renderPassType, _key)
	{

	}

	//--------------------------------------------------------------------------------------
	RenderPass::~RenderPass()
	{

	}

	//--------------------------------------------------------------------------------------
	void RenderPass::finalize()
	{
		super::finalize();
	}
}