#include "BLAS_dx12.h"

namespace vg::gfx::dx12
{
    //--------------------------------------------------------------------------------------
    BLAS::BLAS(BLASUpdateType _blasUpdateType, BLASVariantKey _key) :
        super(_blasUpdateType, _key)
    {

    }

    //--------------------------------------------------------------------------------------
    BLAS::~BLAS()
    {

    }

    //--------------------------------------------------------------------------------------
    void BLAS::addIndexedGeometry(const gfx::Buffer * _ib, core::uint _ibOffset, const core::uint _batchIndexOffset, core::uint _batchIndexCount, const gfx::Buffer * _vb, core::uint _vbOffset, core::uint _vertexCount, core::uint _vbStride, bool _opaque)
    {
        D3D12_RAYTRACING_GEOMETRY_DESC desc{};
        desc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
        desc.Flags = _opaque ? D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE : D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;

        const BufferDesc & vbDesc = _vb->getBufDesc();
        const BufferDesc & ibDesc = _ib->getBufDesc();

        if (BLASUpdateType::Static == m_updateType)
            VG_ASSERT(vbDesc.getElementSize() == _vbStride);

        desc.Triangles.VertexBuffer.StartAddress = _vb->getResource().getd3d12BufferResource()->GetGPUVirtualAddress() + _vbOffset;
        desc.Triangles.VertexBuffer.StrideInBytes = _vbStride;
        desc.Triangles.VertexCount = _vertexCount;
        desc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
        desc.Triangles.IndexBuffer = _ib->getResource().getd3d12BufferResource()->GetGPUVirtualAddress() + _ibOffset + _batchIndexOffset * ibDesc.getElementSize();

        if (ibDesc.getElementSize() == 2)
            desc.Triangles.IndexFormat = DXGI_FORMAT_R16_UINT;
        else
            desc.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;

        desc.Triangles.IndexCount = _batchIndexCount;
        desc.Triangles.Transform3x4 = 0;

        m_DXRGeometries.push_back(desc);
    }

    //--------------------------------------------------------------------------------------
    void BLAS::clear()
    {
        m_DXRGeometries.clear();
    }

    //--------------------------------------------------------------------------------------
    void BLAS::init(bool _update)
    {
        auto device = gfx::Device::get();
        auto d3d12Device = device->getd3d12Device();

        // Get AS build Info
        m_DXRAccelStructInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS flags;
        switch (m_updateType)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(m_updateType);
                break;

            case BLASUpdateType::Static:
                VG_ASSERT(false == _update, "[Device] Cannot update static BLAS");
                flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
                break;

            case BLASUpdateType::Dynamic:
                if (_update)
                    flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_BUILD | D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE | D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;
                else
                    flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_BUILD | D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
                break;
        }

        m_DXRAccelStructInputs.Flags = flags;
        m_DXRAccelStructInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
        m_DXRAccelStructInputs.pGeometryDescs = m_DXRGeometries.data();
        m_DXRAccelStructInputs.NumDescs = (uint)m_DXRGeometries.size();

        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO sizeInfo = {};
        d3d12Device->GetRaytracingAccelerationStructurePrebuildInfo(&m_DXRAccelStructInputs, &sizeInfo);

        // Allocate Buffers for AS
        const auto scratchSize = alignUp((uint)sizeInfo.ScratchDataSizeInBytes, (uint)D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);

        if (_update)
            VG_ASSERT(m_scratchBuffer->getBufDesc().getSize() == scratchSize);

        if (m_scratchBuffer == nullptr || m_scratchBuffer->getBufDesc().getSize() != scratchSize)
        {
            VG_SAFE_RELEASE(m_scratchBuffer);
            BufferDesc scratchBufferDesc(Usage::Default, BindFlags::UnorderedAccess, CPUAccessFlags::None, BufferFlags::None, scratchSize);
            m_scratchBuffer = device->createBuffer(scratchBufferDesc, "BLASScratchBuffer");
        }

        const auto bufferSize = alignUp((uint)sizeInfo.ResultDataMaxSizeInBytes, (uint)D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);

        if (_update)
            VG_ASSERT(m_resultBuffer->getBufDesc().getSize() == bufferSize);

        if (m_resultBuffer == nullptr || m_resultBuffer->getBufDesc().getSize() != bufferSize)
        {
            BufferDesc resultBufferDesc(Usage::Default, BindFlags::RaytracingAccelerationStruct, CPUAccessFlags::None, BufferFlags::None, bufferSize);
            m_resultBuffer = device->createBuffer(resultBufferDesc, "BLASResultBuffer");
        }
    }

    //--------------------------------------------------------------------------------------
    void BLAS::build(gfx::CommandList * _cmdList, bool _update)
    {
        VG_PROFILE_GPU("BLAS");

        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc{};
        desc.Inputs = m_DXRAccelStructInputs;

        if (_update)
            desc.SourceAccelerationStructureData = m_resultBuffer->getResource().getd3d12BufferResource()->GetGPUVirtualAddress();

        desc.ScratchAccelerationStructureData = m_scratchBuffer->getResource().getd3d12BufferResource()->GetGPUVirtualAddress();
        desc.DestAccelerationStructureData = m_resultBuffer->getResource().getd3d12BufferResource()->GetGPUVirtualAddress();
        _cmdList->getd3d12GraphicsCommandList()->BuildRaytracingAccelerationStructure(&desc, 0, nullptr);
        _cmdList->addRWBufferBarrier(m_resultBuffer);
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