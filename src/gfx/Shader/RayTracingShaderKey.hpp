#include "RayTracingShaderKey.h"
#include "gfx/Shader/ShaderManager.h"

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    RayTracingShaderKey::RayTracingShaderKey()
    {
        clear();
    }

    //--------------------------------------------------------------------------------------
    void RayTracingShaderKey::clear()
    {
        rg = RayTracingShaderKey::RG(-1);
        ah = RayTracingShaderKey::AH(-1);
        ch = RayTracingShaderKey::CH(-1);
        mi = RayTracingShaderKey::MI(-1);
        is = RayTracingShaderKey::RG(-1);
        
        flags = RayTracingShaderKey::Flags(0x0000);
    }

    //--------------------------------------------------------------------------------------
    RayTracingShaderKey::RayTracingShaderKey(const core::string & _file, const core::string & _technique)
    {
        clear();
        init(_file, _technique);
    }

    //--------------------------------------------------------------------------------------
    RayTracingShaderKey::~RayTracingShaderKey()
    {

    }

    //--------------------------------------------------------------------------------------
    void RayTracingShaderKey::init(const core::string & _file, const core::string & _technique)
    {
        ShaderManager::get()->initRayTracingShaderKey(*this, _file, _technique);
    }
}