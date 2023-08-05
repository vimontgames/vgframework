#include "gfx/Precomp.h"
#include "RootSignature.h"
#include "gfx/PipelineState/SamplerState.h"
#include "gfx/Device/Device.h"

using namespace vg::core;

#include "RootSignatureDesc.hpp"
#include "RootSignatureTableDesc.hpp"

namespace vg::gfx
{
    namespace base
    {
        //--------------------------------------------------------------------------------------
        RootSignature::RootSignature(const RootSignatureDesc & _desc) :
            m_desc(_desc)
        {

        }

        //--------------------------------------------------------------------------------------
        RootSignature::~RootSignature()
        {

        }
    }
}

#include VG_GFXAPI_IMPL(RootSignature)

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    RootSignature::RootSignature(const RootSignatureDesc & _desc) :
        super::RootSignature(_desc)
    {

    }

    //--------------------------------------------------------------------------------------
    RootSignature::~RootSignature()
    {

    }
}