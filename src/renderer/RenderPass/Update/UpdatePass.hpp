#include "UpdatePass.h"

#include "Update/GPUDebug/GPUDebugUpdatePass.hpp"
#include "Update/InstanceData/InstanceDataUpdatePass.hpp"
#include "Update/MaterialData/MaterialDataUpdatePass.hpp"
#include "Update/TLAS/TLASUpdatePass.hpp"
#include "Update/BLAS/BLASUpdatePass.hpp"
#include "Update/ViewConstants/ViewConstantsUpdatePass.hpp"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    UpdatePass::UpdatePass(const core::string & _name) :
        RenderPass(_name)
    {
        setUserPassType(RenderPassType::Copy);
    }

    //--------------------------------------------------------------------------------------
    UpdatePass::~UpdatePass()
    {

    }
}