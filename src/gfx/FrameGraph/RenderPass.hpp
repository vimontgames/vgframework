#include VG_GFXAPI_IMPL(RenderPass)

namespace vg::gfx
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
		void RenderPass::addSubPass(gfx::SubPass * _subPass)
		{
			m_subPasses.push_back(_subPass);
		}

		//--------------------------------------------------------------------------------------
		const core::vector<gfx::SubPass*> & RenderPass::getSubPasses() const
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