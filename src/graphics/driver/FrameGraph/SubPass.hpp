namespace vg::graphics::driver::base
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
    uint SubPass::getUserPassCount() const
    {
        return (uint)m_userPasses.size();
    }

    //--------------------------------------------------------------------------------------
    const UserPass * SubPass::getUserPass(uint _index) const
    {
        return m_userPasses[_index];
    }
}

#include VG_GFXAPI_IMPL(SubPass)

namespace vg::graphics::driver
{
	//--------------------------------------------------------------------------------------
	SubPass::SubPass()
	{

	}

	//--------------------------------------------------------------------------------------
	SubPass::~SubPass()
	{

	}
}