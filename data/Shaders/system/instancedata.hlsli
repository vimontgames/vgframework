#pragma once

#include "system/packing.hlsli"
#include "system/table.hlsli"

struct GPUMaterialData
{
    #ifdef __cplusplus
    GPUMaterialData()
    {
        setAlbedoTextureHandle(RESERVEDSLOT_TEXSRV_DEFAULT_ALBEDO);
        setNormalTextureHandle(RESERVEDSLOT_TEXSRV_DEFAULT_NORMAL);
        setPBRTextureHandle(RESERVEDSLOT_TEXSRV_DEFAULT_PBR);
    }   
    #endif 

    // .x = albedo | normal 
    // .y = pbr | unused 
    uint4 textures; 

    void setAlbedoTextureHandle(uint _value)
    {
        textures.x = packUint16low(textures.x, _value);
    }
    uint getAlbedoTextureHandle()
    {
        return unpackUint16low(textures.x);
    }

    void setNormalTextureHandle(uint _value)
    {
        textures.x = packUint16high(textures.x, _value);
    }
    uint getNormalTextureHandle()
    {
         return unpackUint16high(textures.x);
    }

    // pbr
    void setPBRTextureHandle(uint _value)
    {
        textures.y = packUint16low(textures.y, _value);
    }
    uint getPBRTextureHandle()
    {
        return unpackUint16low(textures.y);
    }
};

struct GPUInstanceData
{
    uint4 data;

    void setMaterialCount  (uint _count)   { data.x = packUint16low(data.x, _count); }
    uint getMaterialCount  ()              { return unpackUint16low(data.x); }

    #ifndef __cplusplus
    //--------------------------------------------------------------------------------------
    // Get texture handles from instance data. If no instance data specified, offset is 0 and default instance data is used
    // Material entries are only present once material is loaded, if matIndex is above limit then use the default material
    //--------------------------------------------------------------------------------------
    GPUMaterialData getGPUMaterialData  (uint instanceDataOffset, uint _matID)
    {
        uint matIndex = _matID;
        uint matOffset;

        if (matIndex < getMaterialCount())
            matOffset = instanceDataOffset + sizeof(GPUInstanceData) + matIndex * sizeof(GPUInstanceData);
        else
            matOffset = 0 + sizeof(GPUInstanceData) + 0 * sizeof(GPUInstanceData);  
      
        return getBuffer(RESERVEDSLOT_BUFSRV_INSTANCEDATA).Load<GPUMaterialData>(matOffset);
    }
    #endif
};