#include "BLAS_vulkan.h"

namespace vg::gfx::vulkan
{
    //--------------------------------------------------------------------------------------
    BLAS::BLAS() :
        super()
    {

    }

    //--------------------------------------------------------------------------------------
    BLAS::~BLAS()
    {
        gfx::Device * device = gfx::Device::get();
        device->destroyAccelerationStructure(m_VKBLAS, nullptr);
    }

    //--------------------------------------------------------------------------------------
    void BLAS::addIndexedGeometry(gfx::Buffer * _ib, core::uint _ibOffset, core::uint _indexCount, gfx::Buffer * _vb, core::uint _vbOffset, core::uint _vbStride)
    {
        const BufferDesc & vbDesc = _vb->getBufDesc();
        const BufferDesc & ibDesc = _ib->getBufDesc();

        VG_ASSERT(vbDesc.getElementSize() == _vbStride);

        VkAccelerationStructureGeometryKHR desc = {};
        desc.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        desc.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
        desc.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
        desc.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
        desc.geometry.triangles.vertexData.deviceAddress = _vb->getResource().getVulkanDeviceAddress();
        desc.geometry.triangles.vertexStride = _vbStride;
        desc.geometry.triangles.maxVertex = vbDesc.getElementCount();
        desc.geometry.triangles.pNext = nullptr;
        desc.geometry.triangles.transformData.hostAddress = nullptr;
        desc.pNext = nullptr;
        desc.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;

        desc.geometry.triangles.indexData.deviceAddress = _ib->getResource().getVulkanDeviceAddress();

        if (ibDesc.getElementSize() == 2)
            desc.geometry.triangles.indexType = VK_INDEX_TYPE_UINT16;
        else
            desc.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;

        VkAccelerationStructureBuildRangeInfoKHR buildRangeInfo = {};
        buildRangeInfo.primitiveCount = ibDesc.getElementCount() / 3;
        buildRangeInfo.primitiveOffset = 0;
        buildRangeInfo.firstVertex = 0;
        buildRangeInfo.transformOffset = 0;

        m_VKRTBuildRangeInfos.push_back(buildRangeInfo);
        m_VKRTMaxPrimitives.push_back(ibDesc.getElementCount() / 3);
        m_VKRTGeometries.push_back(desc);
    }

    //--------------------------------------------------------------------------------------
    void BLAS::init()
    {
        gfx::Device * device = gfx::Device::get();
        const gfx::DeviceCaps & caps = device->getDeviceCaps();

        // BLAS build Info
        m_VKRTAccelStructInputs.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
        m_VKRTAccelStructInputs.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
        m_VKRTAccelStructInputs.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
        m_VKRTAccelStructInputs.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
        m_VKRTAccelStructInputs.pNext = nullptr;
        m_VKRTAccelStructInputs.geometryCount = static_cast<uint32_t>(m_VKRTGeometries.size());
        m_VKRTAccelStructInputs.pGeometries = m_VKRTGeometries.data();

        VkAccelerationStructureBuildSizesInfoKHR sizeInfo = {};
        sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

        device->getAccelerationStructureBuildSizes(VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &m_VKRTAccelStructInputs, m_VKRTMaxPrimitives.data(), &sizeInfo);

        // alloc BLAS scratch buffer 
        BufferDesc scratchBufferDesc(Usage::Default, BindFlags::UnorderedAccess, CPUAccessFlags::None, BufferFlags::None, (uint)sizeInfo.buildScratchSize, 1, caps.rayTracingAccelerationStructureScratchOffsetAlignment);
        m_scratchBuffer = device->createBuffer(scratchBufferDesc, "TLASScratchBuffer");

        // alloc BLAS result buffer 
        BufferDesc resultBufferDesc(Usage::Default, BindFlags::RaytracingAccelerationStruct, CPUAccessFlags::None, BufferFlags::None, (uint)sizeInfo.accelerationStructureSize);
        m_resultBuffer = device->createBuffer(resultBufferDesc, "TLASResultBuffer");

        // Create BLAS Handle
        VkAccelerationStructureCreateInfoKHR desc{};
        desc.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
        desc.pNext = nullptr;
        
        desc.buffer = m_resultBuffer->getResource().getVulkanBuffer();
        desc.size = sizeInfo.accelerationStructureSize;
        desc.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
        
        VG_VERIFY_VULKAN(device->createAccelerationStructure(&desc, nullptr, &m_VKBLAS));
        
        m_VKRTAccelStructInputs.dstAccelerationStructure = m_VKBLAS;
        m_VKRTAccelStructInputs.scratchData.deviceAddress = m_scratchBuffer->getResource().getVulkanDeviceAddress();
    }

    //--------------------------------------------------------------------------------------
    void BLAS::build(gfx::CommandList * _cmdList)
    {
        VG_PROFILE_GPU("BLAS");

        VkAccelerationStructureBuildRangeInfoKHR * blasRanges[] = { m_VKRTBuildRangeInfos.data() };
        _cmdList->buildAccelerationStructures(1, &m_VKRTAccelStructInputs, blasRanges);
    }
}