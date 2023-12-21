#pragma once

#include "gfx/Raytracing/TLAS.h"

namespace vg::gfx
{
    class CommandList;
    class BLAS;
}

namespace vg::gfx::vulkan
{
    class TLAS : public base::TLAS
    {
        using super = base::TLAS;

    public:
        TLAS();
        ~TLAS();

        void reset();
        void build(gfx::CommandList * _cmdList);
        void addInstance(const gfx::BLAS * _blas, const core::float4x4 & _world, const core::u32 _instanceID);

    private:
        std::vector<VkAccelerationStructureInstanceKHR> m_VKInstances = {};
        VkAccelerationStructureKHR                      m_VKTLAS = VK_NULL_HANDLE;
    };
}