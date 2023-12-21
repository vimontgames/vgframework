#include "BLAS_dx12.h"

namespace vg::gfx::dx12
{
    //--------------------------------------------------------------------------------------
    BLAS::BLAS() :
        super()
    {

    }

    //--------------------------------------------------------------------------------------
    BLAS::~BLAS()
    {

    }

    //--------------------------------------------------------------------------------------
    void BLAS::addIndexedGeometry(gfx::Buffer * _ib, core::uint _ibOffset, core::uint _indexCount, gfx::Buffer * _vb, core::uint _vbOffset, core::uint _vbStride)
    {
        D3D12_RAYTRACING_GEOMETRY_DESC desc{};
        desc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
        desc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

        const BufferDesc & vbDesc = _vb->getBufDesc();
        const BufferDesc & ibDesc = _ib->getBufDesc();

        VG_ASSERT(vbDesc.getElementSize() == _vbStride);

        desc.Triangles.VertexBuffer.StartAddress = _vb->getResource().getd3d12BufferResource()->GetGPUVirtualAddress();
        desc.Triangles.VertexBuffer.StrideInBytes = _vbStride;
        desc.Triangles.VertexCount = vbDesc.getElementCount();
        desc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
        desc.Triangles.IndexBuffer = _ib->getResource().getd3d12BufferResource()->GetGPUVirtualAddress();

        if (ibDesc.getElementSize() == 2)
            desc.Triangles.IndexFormat = DXGI_FORMAT_R16_UINT;
        else
            desc.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;

        desc.Triangles.IndexCount = ibDesc.getElementCount();
        desc.Triangles.Transform3x4 = 0;

        m_DXRGeometries.push_back(desc);
    }

    //--------------------------------------------------------------------------------------
    void BLAS::init()
    {
        auto device = gfx::Device::get();
        auto d3d12Device = device->getd3d12Device();

        // Get AS build Info
        m_DXRAccelStructInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
        m_DXRAccelStructInputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
        m_DXRAccelStructInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
        m_DXRAccelStructInputs.pGeometryDescs = m_DXRGeometries.data();
        m_DXRAccelStructInputs.NumDescs = (uint)m_DXRGeometries.size();

        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO sizeInfo = {};
        d3d12Device->GetRaytracingAccelerationStructurePrebuildInfo(&m_DXRAccelStructInputs, &sizeInfo);

        // Allocate Buffers for AS
        BufferDesc scratchBufferDesc(Usage::Default, BindFlags::UnorderedAccess, CPUAccessFlags::None, BufferFlags::None, alignUp((uint)sizeInfo.ScratchDataSizeInBytes, (uint)D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT));
        m_scratchBuffer = device->createBuffer(scratchBufferDesc, "ScratchBuffer");

        BufferDesc resultBufferDesc(Usage::Default, BindFlags::RaytracingAccelerationStruct, CPUAccessFlags::None, BufferFlags::None, alignUp((uint)sizeInfo.ResultDataMaxSizeInBytes, (uint)D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT));
        m_resultBuffer = device->createBuffer(resultBufferDesc, "ResultBuffer");
    }

    //--------------------------------------------------------------------------------------
    void BLAS::build(gfx::CommandList * _cmdList)
    {
        VG_PROFILE_GPU("BLAS");

        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc{};
        desc.Inputs = m_DXRAccelStructInputs;
        desc.ScratchAccelerationStructureData = m_scratchBuffer->getResource().getd3d12BufferResource()->GetGPUVirtualAddress();
        desc.DestAccelerationStructureData = m_resultBuffer->getResource().getd3d12BufferResource()->GetGPUVirtualAddress();
        _cmdList->getd3d12GraphicsCommandList()->BuildRaytracingAccelerationStructure(&desc, 0, nullptr);
    }
}