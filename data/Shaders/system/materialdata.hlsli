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

        tiling.zw = 0; // unused
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