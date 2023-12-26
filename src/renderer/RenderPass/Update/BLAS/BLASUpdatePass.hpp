#include "BLASUpdatePass.h"
#include "renderer/RayTracing/RayTracingManager.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    BLASUpdatePass::BLASUpdatePass() :
        UpdatePass("RTASUpdatePass")
    {
     
    }

    //--------------------------------------------------------------------------------------
    BLASUpdatePass::~BLASUpdatePass()
    {
      
    }

    //--------------------------------------------------------------------------------------
    void BLASUpdatePass::Setup(const gfx::RenderPassContext & _renderPassContext, float _dt)
    {
        const auto skinningRWBuffer = "SkinningRWBuffer";
        readRWBuffer(skinningRWBuffer);
    }

    //--------------------------------------------------------------------------------------
    void BLASUpdatePass::BeforeRender(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList)
    {
        VG_PROFILE_GPU("BLASUpdate");
        RayTracingManager * rtManager = RayTracingManager::get();
        Buffer * skinningBuffer = getRWBuffer("SkinningRWBuffer");
        rtManager->update(_cmdList, skinningBuffer);
    }
}
