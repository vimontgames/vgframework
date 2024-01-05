#include "TLAS_dx12.h"

namespace vg::gfx::dx12
{
    //--------------------------------------------------------------------------------------
    TLAS::TLAS() :
        super()
    {
        auto device = gfx::Device::get();

        // alloc TLAS scratch buffer 
        BufferDesc scratchBufferDesc(Usage::Default, BindFlags::UnorderedAccess, CPUAccessFlags::None, BufferFlags::None, alignUp((uint)VG_TLAS_SCRATCH_SIZE, (uint)D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT));
        m_scratchBuffer = device->createBuffer(scratchBufferDesc, "TLASScratchBuffer");

        // alloc TLAS result buffer 
        BufferDesc resultBufferDesc(Usage::Default, BindFlags::RaytracingAccelerationStruct, CPUAccessFlags::None, BufferFlags::None, alignUp((uint)VG_TLAS_SIZE, (uint)D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT));
        m_resultBuffer = device->createBuffer(resultBufferDesc, "TLASResultBuffer");

        // alloc TLAS instance buffer
        BufferDesc instanceBufferDesc = BufferDesc(Usage::Default, BindFlags::ShaderResource, CPUAccessFlags::Write, BufferFlags::None, sizeof(D3D12_RAYTRACING_INSTANCE_DESC), VG_TLAS_INSTANCECOUNT);
        m_instanceBuffer = device->createBuffer(instanceBufferDesc, "TLASInstanceBuffer", nullptr);
    }

    //--------------------------------------------------------------------------------------
    TLAS::~TLAS()
    {
     
    }

    //--------------------------------------------------------------------------------------
    void TLAS::reset()
    { 
        m_DXRInstanceDescriptors.clear(); 
    };

    //--------------------------------------------------------------------------------------
    void TLAS::build(gfx::CommandList * _cmdList)
    {
        auto device = gfx::Device::get();
        auto d3d12Device = device->getd3d12Device();

        if (m_DXRInstanceDescriptors.size() > 0)
        {
            VG_ASSERT(m_DXRInstanceDescriptors.size() < VG_TLAS_INSTANCECOUNT, "[Device] TLAS m_instanceBuffer is not big enough");
            const auto mapSizeInBytes = sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * m_DXRInstanceDescriptors.size();
            D3D12_RAYTRACING_INSTANCE_DESC * data = (D3D12_RAYTRACING_INSTANCE_DESC *)_cmdList->map(m_instanceBuffer, mapSizeInBytes).data;
            {
                memcpy(data, m_DXRInstanceDescriptors.data(), mapSizeInBytes);
            }
            _cmdList->unmap(m_instanceBuffer);

            // Get AS build Info
            D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS asInputs = {};
            asInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
            asInputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
            asInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;

            asInputs.NumDescs = static_cast<UINT>(m_DXRInstanceDescriptors.size());
            asInputs.InstanceDescs = m_instanceBuffer->getResource().getd3d12BufferResource()->GetGPUVirtualAddress();

            #ifdef VG_DEBUG
            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO sizeInfo = {};
            d3d12Device->GetRaytracingAccelerationStructurePrebuildInfo(&asInputs, &sizeInfo);
            VG_ASSERT(sizeInfo.ResultDataMaxSizeInBytes < VG_TLAS_SIZE, "[Device] TLAS m_resultBuffer is not big enough");
            VG_ASSERT(sizeInfo.ScratchDataSizeInBytes < VG_TLAS_SCRATCH_SIZE, "[Device] TLAS m_scratchBuffer not big enough");
            #endif

            D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc{};
            desc.Inputs = asInputs;
            desc.ScratchAccelerationStructureData = m_scratchBuffer->getResource().getd3d12BufferResource()->GetGPUVirtualAddress();
            desc.DestAccelerationStructureData = m_resultBuffer->getResource().getd3d12BufferResource()->GetGPUVirtualAddress();

            D3D12GraphicsCommandList * d3d12CmdList = _cmdList->getd3d12GraphicsCommandList();
            d3d12CmdList->BuildRaytracingAccelerationStructure(&desc, 0, nullptr);

            _cmdList->addRWBufferBarrier(m_resultBuffer);

            BindlessTable * bindlessTable = gfx::Device::get()->getBindlessTable();

            if (m_bindlessTLASHandle.isValid())
                bindlessTable->updateBindlessTLASHandle(m_bindlessTLASHandle, static_cast<gfx::TLAS *>(this));
        }
    }

    //--------------------------------------------------------------------------------------
    void TLAS::addInstance(const gfx::BLAS * _blas, const core::float4x4 & _world, const core::u32 _instanceID)
    {
        D3D12_RAYTRACING_INSTANCE_DESC desc = {};
        memcpy(desc.Transform, &(float&)transpose(_world), sizeof(desc.Transform));
        desc.InstanceID = _instanceID;
        desc.InstanceMask = 0xFF;
        desc.InstanceContributionToHitGroupIndex = 0;
        desc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;

        desc.AccelerationStructure = _blas->getBuffer()->getResource().getd3d12BufferResource()->GetGPUVirtualAddress();

        m_DXRInstanceDescriptors.emplace_back(std::move(desc));
    }
}