namespace vg::gfx::base
{
	//--------------------------------------------------------------------------------------
	SubPass::SubPass()
	{

	}

	//--------------------------------------------------------------------------------------
	SubPass::~SubPass()
	{

	}

    //--------------------------------------------------------------------------------------
    void SubPass::addUserPassInfo(const FrameGraph::UserPassInfo & _userPassInfo)
    {
        m_userPassesInfos.push_back(_userPassInfo);
    }

    //--------------------------------------------------------------------------------------
	const core::vector<FrameGraph::UserPassInfo> & SubPass::getUserPassesInfos() const
    {
        return m_userPassesInfos;
    }
}

#include VG_GFXAPI_IMPL(SubPass)

namespace vg::gfx
{
	//--------------------------------------------------------------------------------------
	SubPass::SubPass() :
        super::SubPass()
	{

	}

	//--------------------------------------------------------------------------------------
	SubPass::~SubPass()
	{

	}
}