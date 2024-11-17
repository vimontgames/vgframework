#pragma once

#include "system/packing.hlsli"
#include "system/table.hlsli"
#include "system/materialdata.hlsli"
#include "system/vertex.hlsli"

#define GPU_INSTANCE_DATA_ALIGNMENT 8

//--------------------------------------------------------------------------------------
// 1xGPUInstanceData, N*GPUBatchData : In instance stream, 1 'GPUInstanceData' is followed by N 'GPUBatchData' for every batch (<=>material)
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// GPUInstanceData
// Materials use 16-bits indices and up to 16 materials per instance are supported
//--------------------------------------------------------------------------------------
struct GPUBatchData
{
    uint m_material;
    uint m_startIndex;

    void setMaterialIndex               (uint _handle)                                      { m_material = _handle; }
    uint getMaterialBytesOffset         ()                                                  { return m_material * sizeof(GPUMaterialData); }

    void setStartIndex                  (uint _startIndex)                                  { m_startIndex = _startIndex; }
    uint getStartIndex                  ()                                                  { return m_startIndex; }

    #ifndef __cplusplus
    GPUMaterialData loadGPUMaterialData  ()                                                  { return getBuffer(RESERVEDSLOT_BUFSRV_MATERIALDATA).Load<GPUMaterialData>(getMaterialBytesOffset()); }
    #endif
};

//--------------------------------------------------------------------------------------
// GPUInstanceData
// Materials use 16-bits indices and up to 16 materials per instance are supported
//--------------------------------------------------------------------------------------
struct GPUInstanceData
{
    uint4 m_header;

    void setMaterialCount               (uint _count)                                       { m_header.x = packR8(m_header.x, _count); }
    uint getMaterialCount               ()                                                  { return unpackR8(m_header.x); }

    void setVertexFormat                (VertexFormat _vertexFormat)                        { m_header.x = packG8(m_header.x, (uint)_vertexFormat); }
    VertexFormat getVertexFormat        ()                                                  { return (VertexFormat)unpackG8(m_header.x); }

    void setInstanceColor               (float4 _color)                                     { m_header.y = packRGBA8(_color); }
    float4 getInstanceColor             ()                                                  { return unpackRGBA8(m_header.y); }

    void setIndexBuffer                 (uint _ib, uint _indexSize = 2, uint _offset = 0)   { m_header.z = packUint16low(m_header.z, _indexSize == 4 ? (_ib | 0x8000) : _ib); }
    uint getIndexBufferHandle           ()                                                  { return unpackUint16low(m_header.z) & ~0x8000; }
    uint getIndexSize                   ()                                                  { return (unpackUint16low(m_header.z) & 0x8000) ? 4 : 2; }

    void setVertexBuffer                (uint _vb, uint _offset = 0)                        { m_header.z = packUint16high(m_header.z, _vb); m_header.w = _offset; }
    uint getVertexBufferHandle          ()                                                  { return unpackUint16high(m_header.z); }
    uint getVertexBufferOffset          ()                                                  { return m_header.w; }

    #ifndef __cplusplus

    GPUBatchData loadGPUBatchData        (uint _instanceDataOffset, uint _batchIndex)
    {
        GPUBatchData batchData = (GPUBatchData)0;

        uint batchDataOffset = 0;
        if (_batchIndex < getMaterialCount())
            batchData = getBuffer(RESERVEDSLOT_BUFSRV_INSTANCEDATA).Load<GPUBatchData>(_instanceDataOffset + sizeof(GPUInstanceData) + _batchIndex * sizeof(GPUBatchData));

        return batchData;
    }

    GPUMaterialData loadGPUMaterialData  (uint _instanceDataOffset, uint _matID)
    {
        GPUBatchData batchData = loadGPUBatchData(_instanceDataOffset, _matID);
        return batchData.loadGPUMaterialData();
    }
    #endif
};