#pragma once

#include "system/packing.hlsli"
#include "system/table.hlsli"
#include "system/material_consts.hlsli"

struct GPUMaterialData
{
    #ifdef __cplusplus
    GPUMaterialData()
    {
        setAlbedoTextureHandle(RESERVEDSLOT_TEXSRV_DEFAULT_ALBEDO);
        setNormalTextureHandle(RESERVEDSLOT_TEXSRV_DEFAULT_NORMAL);
        setPBRTextureHandle(RESERVEDSLOT_TEXSRV_DEFAULT_PBR);
        
        setAlbedoColor(float4(1,1,1,1));
        setNormalStrength(1.0f);
        setOcclusion(1.0f);
        setRoughness(0.25f);
        setMetalness(0.0f);
        setTiling(float2(1.0f, 1.0f));
    }   
    #endif 

    uint4 textures;         // .x = albedo | normal, .y = pbr | unused, .y = unused, .w = flags
    float4 albedoColor;
    float4 data;
    float4 tiling;

    void        setAlbedoTextureHandle  (uint _value)   { textures.x = packUint16low(textures.x, _value); }
    uint        getAlbedoTextureHandle  ()              { return unpackUint16low(textures.x); }

    void        setNormalTextureHandle  (uint _value)   { textures.x = packUint16high(textures.x, _value); }
    uint        getNormalTextureHandle  ()              { return unpackUint16high(textures.x); }

    void        setPBRTextureHandle     (uint _value)   { textures.y = packUint16low(textures.y, _value); }
    uint        getPBRTextureHandle     ()              { return unpackUint16low(textures.y); }

    void        setUVSource             (UVSource _value) { textures.w = packA8(textures.w, (uint)_value); }
    UVSource    getUVSource             ()              { return (UVSource)unpackA8(textures.w);}

    void        setAlbedoColor          (float4 _value) { albedoColor = _value; }
    float4      getAlbedoColor          ()              { return albedoColor; }
    
    void        setNormalStrength       (float _value)  { data.w = _value; }
    float       getNormalStrength       ()              { return data.w; }
    
    void        setOcclusion            (float _value)  { data.x = _value; }
    float       getOcclusion            ()              { return data.x; }
    
    void        setRoughness            (float _value)  { data.y = _value; }
    float       getRoughness            ()              { return data.y; }
    
    void        setMetalness            (float _value)  { data.z = _value; }
    float       getMetalness            ()              { return data.z; }

    void        setTiling               (float2 _value) { tiling.xy = _value; }
    float2      getTiling               ()              { return tiling.xy; }
};

struct GPUInstanceData
{
    uint4 data;

    void setMaterialCount           (uint _count)   { data.x = packUint16low(data.x, _count); }
    uint getMaterialCount           ()              { return unpackUint16low(data.x); }

    void setInstanceColor           (float4 _color) { data.y = packRGBA8(_color); }
    float4 getInstanceColor         ()              { return unpackRGBA8(data.y); }

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
            matOffset = instanceDataOffset + sizeof(GPUInstanceData) + matIndex * sizeof(GPUMaterialData);
        else
            matOffset = 0 + sizeof(GPUInstanceData) + 0 * sizeof(GPUMaterialData);  
      
        return getBuffer(RESERVEDSLOT_BUFSRV_INSTANCEDATA).Load<GPUMaterialData>(matOffset);
    }
    #endif
};