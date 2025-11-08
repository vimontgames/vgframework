#include "BLAS_vulkan.h"

namespace vg::gfx::vulkan
{
    //--------------------------------------------------------------------------------------
    BLAS::BLAS(BLASUpdateType _blasUpdateType, gfx::BLASVariantKey _key) :
        super(_blasUpdateType, _key)
    {

    }

    //--------------------------------------------------------------------------------------
    BLAS::~BLAS()
    {
        gfx::Device * device = gfx::Device::get();
        device->destroyAccelerationStructure(m_VKBLAS, nullptr);
    }

    //--------------------------------------------------------------------------------------
    void BLAS::addIndexedGeometry(const gfx::Buffer * _ib, core::uint _ibOffset, const core::uint _batchIndexOffset, core::uint _batchIndexCount, const gfx::Buffer * _vb, core::uint _vbOffset, core::uint _vertexCount, core::uint _vbStride, bool _opaque)
    {
        const BufferDesc & vbDesc = _vb->getBufDesc();
        const BufferDesc & ibDesc = _ib->getBufDesc();

        if (BLASUpdateType::Static == m_updateType)
            VG_ASSERT(vbDesc.getElementSize() == _vbStride);

        VkAccelerationStructureGeometryKHR desc = {};
        desc.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        desc.flags = _opaque ? VK_GEOMETRY_OPAQUE_BIT_KHR : 0x0;
        desc.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
        desc.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
        desc.geometry.triangles.vertexData.deviceAddress = _vb->getResource().getVulkanDeviceAddress() + _vbOffset;
        desc.geometry.triangles.vertexStride = _vbStride;
        desc.geometry.triangles.maxVertex = _vertexCount;
        desc.geometry.triangles.pNext = nullptr;
        desc.geometry.triangles.transformData.hostAddress = nullptr;
        desc.pNext = nullptr;
        desc.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;

        desc.geometry.triangles.indexData.deviceAddress = _ib->getResource().getVulkanDeviceAddress() + _ibOffset + _batchIndexOffset * ibDesc.getElementSize();

        if (ibDesc.getElementSize() == 2)
            desc.geometry.triangles.indexType = VK_INDEX_TYPE_UINT16;
        else
            desc.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;

        VkAccelerationStructureBuildRangeInfoKHR buildRangeInfo = {};
        buildRangeInfo.primitiveCount = _batchIndexCount / 3;
        buildRangeInfo.primitiveOffset = 0;// _batchIndexOffset * ibDesc.getElementSize();// (_batchIndexOffset / 3) * ibDesc.getElementSize();
        buildRangeInfo.firstVertex = 0;
        buildRangeInfo.transformOffset = 0;

        m_VKRTBuildRangeInfos.push_back(buildRangeInfo);
        m_VKRTMaxPrimitives.push_back(_batchIndexCount / 3);
        m_VKRTGeometries.push_back(desc);
    }

    //--------------------------------------------------------------------------------------
    void BLAS::clear()
    {
        m_VKRTGeometries.clear();
    }

    //--------------------------------------------------------------------------------------
    void BLAS::init(bool _update)
    {
        gfx::Device * device = gfx::Device::get(); 
        const gfx::DeviceCaps & caps = device->getDeviceCaps();

        // BLAS build Info
        m_VKRTAccelStructInputs.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
        m_VKRTAccelStructInputs.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;

        VkBuildAccelerationStructureModeKHR mode;
        VkBuildAccelerationStructureFlagsKHR flags;
        VkAccelerationStructureKHR src = VK_NULL_HANDLE;
        switch (m_updateType)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(m_updateType);
                break;

            case BLASUpdateType::Static:
            {
                VG_ASSERT(false == _update, "[Device] Cannot update static BLAS");
                mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
                flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;

            }
            break;

            case BLASUpdateType::Dynamic:
            {
                if (_update)
                {
                    mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR;
                    flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;
                    src = m_VKBLAS;
                }
                else
                {
                    mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
                    flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;
                }
            }
            break;
        }

        m_VKRTAccelStructInputs.mode = mode;
        m_VKRTAccelStructInputs.flags = flags;
        m_VKRTAccelStructInputs.pNext = nullptr;
        m_VKRTAccelStructInputs.geometryCount = static_cast<uint32_t>(m_VKRTGeometries.size());
        m_VKRTAccelStructInputs.pGeometries = m_VKRTGeometries.data();

        VkAccelerationStructureBuildSizesInfoKHR sizeInfo = {};
        sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

        device->getAccelerationStructureBuildSizes(VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &m_VKRTAccelStructInputs, m_VKRTMaxPrimitives.data(), &sizeInfo);

        // alloc BLAS scratch buffer 
        const auto scratchSize = sizeInfo.buildScratchSize;

        if (_update)
            VG_ASSERT(m_scratchBuffer->getBufDesc().getSize() == scratchSize);

        if (m_scratchBuffer == nullptr || m_scratchBuffer->getBufDesc().getSize() != scratchSize)
        {
            BufferDesc scratchBufferDesc(Usage::Default, BindFlags::UnorderedAccess, CPUAccessFlags::None, BufferFlags::None, (uint)scratchSize, 1, caps.vulkan.rayTracingAccelerationStructureScratchOffsetAlignment);
            m_scratchBuffer = device->createBuffer(scratchBufferDesc, "TLASScratchBuffer");
        }

        // alloc BLAS result buffer 
        const auto bufferSize = sizeInfo.accelerationStructureSize;

        if (_update)
            VG_ASSERT(m_resultBuffer->getBufDesc().getSize() == bufferSize);

        if (m_resultBuffer == nullptr || m_resultBuffer->getBufDesc().getSize() != bufferSize)
        {
            BufferDesc resultBufferDesc(Usage::Default, BindFlags::RaytracingAccelerationStruct, CPUAccessFlags::None, BufferFlags::None, (uint)bufferSize);
            m_resultBuffer = device->createBuffer(resultBufferDesc, "TLASResultBuffer");
        }

        // Create BLAS Handle
        VkAccelerationStructureCreateInfoKHR desc{};
        desc.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
        desc.pNext = nullptr;
        
        desc.buffer = m_resultBuffer->getResource().getVulkanBuffer();
        desc.size = sizeInfo.accelerationStructureSize;
        desc.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;

        if (_update)
        {
            m_VKRTAccelStructInputs.srcAccelerationStructure = src;
            m_VKRTAccelStructInputs.dstAccelerationStructure = m_VKBLAS;

            VG_ASSERT(m_VKRTAccelStructInputs.srcAccelerationStructure == m_VKRTAccelStructInputs.dstAccelerationStructure);
        }
        else
        {
            VG_VERIFY_VULKAN(device->createAccelerationStructure(&desc, nullptr, &m_VKBLAS));

            m_VKRTAccelStructInputs.srcAccelerationStructure = VK_NULL_HANDLE;
            m_VKRTAccelStructInputs.dstAccelerationStructure = m_VKBLAS;

            VG_ASSERT(m_VKRTAccelStructInputs.srcAccelerationStructure != m_VKRTAccelStructInputs.dstAccelerationStructure);
        }
        
        m_VKRTAccelStructInputs.scratchData.deviceAddress = m_scratchBuffer->getResource().getVulkanDeviceAddress();
    }

    //--------------------------------------------------------------------------------------
    void BLAS::build(gfx::CommandList * _cmdList, bool _update)
    {
        VG_PROFILE_GPU("BLAS");

        VkAccelerationStructureBuildRangeInfoKHR * blasRanges[] = { m_VKRTBuildRangeInfos.data() };

        _cmdList->buildAccelerationStructures(1, &m_VKRTAccelStructInputs, blasRanges);
    }

    //--------------------------------------------------------------------------------------
    void BLAS::update(gfx::CommandList * _cmdList)
    {
        bool update = m_initDone;
        init(update);
        build(_cmdList, update);
        m_initDone = true;
    }
}