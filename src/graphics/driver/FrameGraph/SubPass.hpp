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
    void SubPass::addUserPass(const UserPass * _userPass)
    {
        m_userPasses.push_back(_userPass);
    }

    //--------------------------------------------------------------------------------------
    const core::vector<const UserPass*> & SubPass::getUserPasses() const
    {
        return m_userPasses;
    }
}

#include VG_GFXAPI_IMPL(SubPass)

namespace vg::graphics::driver
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