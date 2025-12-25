#ifndef _INSTANCE_DATA__HLSLI_
#define _INSTANCE_DATA__HLSLI_

#include "system/packing.hlsli"
#include "system/table.hlsli"
#include "system/materialdata.hlsli"
#include "system/vertex.hlsli"

#define GPU_INSTANCE_DATA_ALIGNMENT 16

vg_enum_class_global(GPUInstanceFlags, u8,
    Mesh     = 0x00,
    Particle = 0x01,

    Static   = 0x80
);

#define GPUInstanceFlags_TypeMask 0x0F

//--------------------------------------------------------------------------------------
// 1xGPUInstanceData, N*GPUBatchData : In instance stream, 1 'GPUInstanceData' is followed by N 'GPUBatchData' for every batch (1 batch <=> 1 material)
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// GPUBatchData
// material index is 16 bits, but startIndex can be 32 bits to support large meshes and shared vertex buffer used by skinning
//--------------------------------------------------------------------------------------
struct GPUBatchData
{
    uint m_material;    // Only low 16 bits are actually used
    uint m_startIndex;  // Full 32 bits potentially used

    void setMaterialIndex               (uint _handle)                                      { m_material = packUint16low(m_material, _handle); }
    uint getMaterialBytesOffset         ()                                                  { return unpackUint16low(m_material) * sizeof(GPUMaterialData); }

    void setStartIndex                  (uint _startIndex)                                  { m_startIndex = _startIndex; }
    uint getStartIndex                  ()                                                  { return m_startIndex; }

    #ifndef __cplusplus
    GPUMaterialData loadGPUMaterialData  ()                                                 { return getBuffer(RESERVEDSLOT_BUFSRV_MATERIALDATA).Load<GPUMaterialData>(getMaterialBytesOffset()); }
    #endif
};

//--------------------------------------------------------------------------------------
// GPUInstanceData
//--------------------------------------------------------------------------------------
struct GPUInstanceData
{
    // m_header[0]: MaterialCount | VertexFormat<<8 | unused<<16 | GPUInstanceFlags (8 bits x 4)
    // m_header[1]: InstanceColor                                                   (32 bits)
    // m_header[2]: (IndexSize + IB) | VB                                           (16 bits x 2)
    // m_header[3]: VBOffset                                                        (32 bits)
    uint m_header[4];   

    void                setMaterialCount        (uint _count)                                       { m_header[0] = packR8(m_header[0], _count); }
    uint                getMaterialCount        ()                                                  { return unpackR8(m_header[0]); }

    void                setVertexFormat         (VertexFormat _vertexFormat)                        { m_header[0] = packG8(m_header[0], (uint)_vertexFormat); }
    VertexFormat        getVertexFormat         ()                                                  { return (VertexFormat)unpackG8(m_header[0]); }

    void                setGPUInstanceFlags     (GPUInstanceFlags _value)                           { m_header[0] = packA8(m_header[0], (uint)_value); } 
    GPUInstanceFlags    getGPUInstanceFlags     ()                                                  { return (GPUInstanceFlags) unpackA8(m_header[0]); }
 
    void                setInstanceColor        (float4 _color)                                     { m_header[1] = packRGBA8(_color); }
    float4              getInstanceColor        (DisplayFlags _flags)                               { return (IsToolMode() && 0 == (DisplayFlags::InstanceColor & _flags)) ? (float4)1.0f : unpackRGBA8(m_header[1]); }

    void                setIndexBuffer          (uint _ib, uint _indexSize = 2, uint _offset = 0)   { m_header[2] = packUint16low(m_header[2], _indexSize == 4 ? (_ib | 0x8000) : _ib); }
    uint                getIndexBufferHandle    ()                                                  { return unpackUint16low(m_header[2]) & ~0x8000; }
    uint                getIndexSize            ()                                                  { return (unpackUint16low(m_header[2]) & 0x8000) ? 4 : 2; }

    void                setVertexBuffer         (uint _vb, uint _offset = 0)                        { m_header[2] = packUint16high(m_header[2], _vb); m_header[3] = _offset; }
    uint                getVertexBufferHandle   ()                                                  { return unpackUint16high(m_header[2]); }
    uint                getVertexBufferOffset   ()                                                  { return m_header[3]; }

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

#endif // _INSTANCE_DATA__HLSLI_