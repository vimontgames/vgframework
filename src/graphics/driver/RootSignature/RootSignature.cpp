#include "graphics/driver/Precomp.h"
#include "RootSignature.h"
#include "graphics/driver/Device/Device.h"

using namespace vg::core;

#include "RootSignatureDesc.hpp"
#include "RootSignatureTableDesc.hpp"

namespace vg::graphics::driver
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

namespace vg::graphics::driver
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