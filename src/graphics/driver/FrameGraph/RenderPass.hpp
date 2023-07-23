#include VG_GFXAPI_IMPL(RenderPass)

namespace vg::graphics::driver
{
	namespace base
	{
		//--------------------------------------------------------------------------------------
		RenderPass::RenderPass(const RenderPassKey & _key) :
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

    core::Pool<RenderPass> RenderPass::s_memPool;

	//--------------------------------------------------------------------------------------
	RenderPass::RenderPass(const RenderPassKey & _key) :
        super::RenderPass(_key)
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