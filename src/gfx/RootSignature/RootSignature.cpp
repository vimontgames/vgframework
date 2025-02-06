#include "gfx/Precomp.h"
#include "RootSignature.h"
#include "gfx/PipelineState/Graphic/SamplerState.h"
#include "gfx/Device/Device.h"

#include "RootSignatureDesc.hpp"
#include "RootSignatureTableDesc.hpp"

#ifdef VG_DX12
#include "dx12/RootSignature_dx12.hpp"
#elif defined(VG_VULKAN)
#include "vulkan/RootSignature_vulkan.hpp"
#else
#error Undefined GFXAPI
#endif

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