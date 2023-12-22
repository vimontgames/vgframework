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
    
    }

    //--------------------------------------------------------------------------------------
    void BLASUpdatePass::BeforeRender(const gfx::RenderPassContext & _renderPassContext, gfx::CommandList * _cmdList)
    {
        RayTracingManager * rtManager = RayTracingManager::get();
        rtManager->update(_cmdList);
    }
}
