#pragma once

#include "gfx/Raytracing/BLAS.h"

namespace vg::gfx
{
    class CommandList;
}

namespace vg::gfx::vulkan
{
    class BLAS : public base::BLAS
    {
        using super = base::BLAS;

    public:
        BLAS();
        ~BLAS();

        void addIndexedGeometry(gfx::Buffer * _ib, core::uint _ibOffset, core::uint _indexCount, gfx::Buffer * _vb, core::uint _vbOffset, core::uint _vbStride);
        void init();
        void build(gfx::CommandList * _cmdList);

    private:
        core::vector<VkAccelerationStructureGeometryKHR>        m_VKRTGeometries = {};
        core::vector<uint32_t>                                  m_VKRTMaxPrimitives = {};
        core::vector<VkAccelerationStructureBuildRangeInfoKHR>  m_VKRTBuildRangeInfos = {};

        VkAccelerationStructureBuildGeometryInfoKHR             m_VKRTAccelStructInputs = {};
        VkAccelerationStructureKHR                              m_VKBLAS = VK_NULL_HANDLE;
    };
}