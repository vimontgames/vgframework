#pragma once

#include "system/packing.hlsli"
#include "system/table.hlsli"
#include "system/materialdata.hlsli"

#define MAX_MATERIAL_PER_INSTANCE 16

//--------------------------------------------------------------------------------------
// GPUInstanceData
// Materials use 16-bits indices and up to 16 materials per instance are supported
//--------------------------------------------------------------------------------------
struct GPUInstanceData
{
    uint4 m_header;
    uint  m_materials[MAX_MATERIAL_PER_INSTANCE];

    void setMaterialCount           (uint _count)   { m_header.x = packUint16low(m_header.x, _count); }
    uint getMaterialCount           ()              { return unpackUint16low(m_header.x); }

    void setInstanceColor           (float4 _color) { m_header.y = packRGBA8(_color); }
    float4 getInstanceColor         ()              { return unpackRGBA8(m_header.y); }

    void setMaterialHandle          (uint _index, uint _handle) { m_materials[_index] = _handle; }
    uint getMaterialHandle          (uint _index)               { return _index < MAX_MATERIAL_PER_INSTANCE ? m_materials[_index] : 0; }

    #ifndef __cplusplus
    //--------------------------------------------------------------------------------------
    // Get texture handles from instance data. If no instance data specified, offset is 0 and default instance data is used
    // Material entries are only present once material is loaded, if matIndex is above limit then use the default material
    //--------------------------------------------------------------------------------------
    GPUMaterialData getGPUMaterialData  (uint instanceDataOffset, uint _matID)
    {
        uint matIndex = getMaterialHandle(_matID);
        uint matOffset = matIndex * sizeof(GPUMaterialData);
        return getBuffer(RESERVEDSLOT_BUFSRV_MATERIALDATA).Load<GPUMaterialData>(matOffset);
    }
    #endif
};