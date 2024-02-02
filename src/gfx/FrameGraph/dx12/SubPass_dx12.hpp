#pragma once

#include "gfx/FrameGraph/SubPass.h"

namespace vg::gfx::dx12
{
	//--------------------------------------------------------------------------------------
	SubPass::SubPass() :
        super::SubPass(),
		m_renderTargetCount(0),
		m_depthStencilCount(0)
	{

	}

	//--------------------------------------------------------------------------------------
	SubPass::~SubPass()
	{

	}
}