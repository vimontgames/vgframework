#include "TLAS_vulkan.h"

namespace vg::gfx::vulkan
{
    //--------------------------------------------------------------------------------------
    TLAS::TLAS() :
        super()
    {
        gfx::Device * device = gfx::Device::get();
        const gfx::DeviceCaps & caps = device->getDeviceCaps();

        // alloc TLAS scratch buffer 
        BufferDesc scratchBufferDesc(Usage::Default, BindFlags::UnorderedAccess, CPUAccessFlags::None, BufferFlags::None, VG_TLAS_SCRATCH_SIZE, 1, caps.vulkan.rayTracingAccelerationStructureScratchOffsetAlignment);
        m_scratchBuffer = device->createBuffer(scratchBufferDesc, "TLASScratchBuffer");

        // alloc TLAS result buffer 
        BufferDesc resultBufferDesc(Usage::Default, BindFlags::RaytracingAccelerationStruct, CPUAccessFlags::None, BufferFlags::None, VG_TLAS_SIZE);
        m_resultBuffer = device->createBuffer(resultBufferDesc, "TLASResultBuffer");

        // alloc TLAS instance buffer
        BufferDesc instanceBufferDesc = BufferDesc(Usage::Default, BindFlags::ConstantBuffer, CPUAccessFlags::Write, BufferFlags::None, sizeof(VkAccelerationStructureInstanceKHR), VG_TLAS_INSTANCECOUNT);
        m_instanceBuffer = device->createBuffer(instanceBufferDesc, "TLASInstanceBuffer", nullptr);

        // Create AS Handle
        VkAccelerationStructureCreateInfoKHR desc = {};
        desc.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
        desc.pNext = nullptr;
        
        desc.buffer = m_resultBuffer->getResource().getVulkanBuffer();
        desc.size = VG_TLAS_SIZE;
        desc.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
        
        VG_VERIFY_VULKAN(device->createAccelerationStructure(&desc, nullptr, &m_VKTLAS));
    }

    //--------------------------------------------------------------------------------------
    TLAS::~TLAS()
    {
        gfx::Device * device = gfx::Device::get();
        device->destroyAccelerationStructure(m_VKTLAS, nullptr);
    }

    //--------------------------------------------------------------------------------------
    void TLAS::reset()
    {
        m_VKInstances.clear();
    };

    //--------------------------------------------------------------------------------------
    void TLAS::build(gfx::CommandList * _cmdList)
    {
        if (m_VKInstances.empty())
            return;

        // Update instances
        auto device = gfx::Device::get();

        VG_ASSERT(m_VKInstances.size() < VG_TLAS_INSTANCECOUNT, "[Device] TLAS m_instanceBuffer is not big enough");

        auto sizeInBytes = sizeof(VkAccelerationStructureInstanceKHR) * m_VKInstances.size();
        VkAccelerationStructureInstanceKHR * data = (VkAccelerationStructureInstanceKHR *)_cmdList->map(m_instanceBuffer, sizeInBytes).data;
        {
            memcpy(data, m_VKInstances.data(), sizeInBytes);
        }
        _cmdList->unmap(m_instanceBuffer);

        // TLAS info
        VkAccelerationStructureGeometryKHR tlasGeometry{};
        tlasGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        tlasGeometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
        tlasGeometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
        tlasGeometry.geometry.instances.arrayOfPointers = VK_FALSE;
        tlasGeometry.geometry.instances.data.deviceAddress = m_instanceBuffer->getResource().getVulkanDeviceAddress();

        VkAccelerationStructureBuildGeometryInfoKHR asInputs = {};
        asInputs.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
        asInputs.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
        asInputs.geometryCount = 1;
        asInputs.pGeometries = &tlasGeometry;
        asInputs.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
        asInputs.pNext = nullptr;
        asInputs.dstAccelerationStructure = m_VKTLAS;
        asInputs.scratchData.deviceAddress = m_scratchBuffer->getResource().getVulkanDeviceAddress();

        VkAccelerationStructureBuildRangeInfoKHR tlasRangeInfo{};
        tlasRangeInfo.primitiveCount = static_cast<uint32_t>(m_VKInstances.size());

        VkAccelerationStructureBuildRangeInfoKHR * tlasRanges[] = { &tlasRangeInfo };

        _cmdList->buildAccelerationStructures(1, &asInputs, tlasRanges);

        _cmdList->addRWBufferBarrier(m_resultBuffer);

        BindlessTable * bindlessTable = gfx::Device::get()->getBindlessTable();
        if (m_bindlessTLASHandle.isValid())
            bindlessTable->updateBindlessTLASHandle(m_bindlessTLASHandle, static_cast<gfx::TLAS *>(this));
    }

    //--------------------------------------------------------------------------------------
    void TLAS::addInstance(const gfx::BLAS * _blas, const core::float4x4 & _world, const core::u32 _instanceID, TLASInstanceFlags _flags)
    {
        VkAccelerationStructureInstanceKHR tlasStructure = {};
        const float4x4 worldT = transpose(_world);
        memcpy(&tlasStructure.transform, (void **)&worldT, sizeof(tlasStructure));
        tlasStructure.mask = 0xff;
        tlasStructure.instanceCustomIndex = _instanceID;
        tlasStructure.instanceShaderBindingTableRecordOffset = 0; 
        tlasStructure.accelerationStructureReference = _blas->getBuffer()->getResource().getVulkanDeviceAddress();

        VG_ASSERT(!asBool(TLASInstanceFlags::Opaque & _flags) || !asBool(TLASInstanceFlags::NotOpaque & _flags));
        if (asBool(TLASInstanceFlags::Opaque & _flags))
            tlasStructure.flags = VK_GEOMETRY_INSTANCE_FORCE_OPAQUE_BIT_KHR;
        else if (asBool(TLASInstanceFlags::NotOpaque & _flags))
            tlasStructure.flags = VK_GEOMETRY_INSTANCE_FORCE_NO_OPAQUE_BIT_KHR;
        else
            tlasStructure.flags = 0x0;

        m_VKInstances.push_back(tlasStructure);
    }
}