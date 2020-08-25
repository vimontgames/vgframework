#include VG_GRAPHICSAPI_IMPLEMENTATION(RenderPass)

namespace vg::graphics::driver
{
	namespace base
	{
		//--------------------------------------------------------------------------------------
		RenderPass::RenderPass()
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
		void RenderPass::addSubPass(driver::SubPass * _subPass)
		{
			m_subPasses.push_back(_subPass);
		}

		//--------------------------------------------------------------------------------------
		const core::vector<driver::SubPass*> & RenderPass::getSubPasses() const
		{
			return m_subPasses;
		}
	}

	//--------------------------------------------------------------------------------------
	RenderPass::RenderPass()
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