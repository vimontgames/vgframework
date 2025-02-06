#ifdef VG_DX12
#include "dx12/SubPass_dx12.hpp"
#elif defined(VG_VULKAN)
#include "vulkan/SubPass_vulkan.hpp"
#else
#error Undefined GFXAPI
#endif

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
    void SubPass::addUserPassInfo(const UserPassInfo & _userPassInfo)
    {
        m_userPassesInfos.push_back(_userPassInfo);
    }

    //--------------------------------------------------------------------------------------
	const core::vector<UserPassInfo> & SubPass::getUserPassesInfos() const
    {
        return m_userPassesInfos;
    }
}

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