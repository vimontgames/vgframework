#pragma once

#include "system/packing.hlsli"
#include "system/table.hlsli"
#include "system/material_consts.hlsli"

#if 1
#define albedoSampler anisotropicRepeat 
#else
#define albedoSampler linearRepeat 
#endif

#if 0
#define normalSampler anisotropicRepeat 
#else
#define normalSampler linearRepeat 
#endif

#if 0
#define pbrSampler anisotropicRepeat 
#else
#define pbrSampler linearRepeat 
#endif

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

    #ifndef __cplusplus
    //--------------------------------------------------------------------------------------
    float4 getVertexColorOut(float4 _vertexColor, float4 _instanceColor, DisplayFlags _flags)
    {
        float4 color = 1;

        #if _TOOLMODE
        if (0 != (DisplayFlags::VertexColor & _flags)) 
        #endif
        {
            color.rgb *= _vertexColor.rgb;
        }
        color.a *= _vertexColor.a;

        float4 _materialColor = getAlbedoColor();
        #if _TOOLMODE
        if (0 != (DisplayFlags::MaterialColor & _flags)) 
        #endif
        {
            color.rgb *= _materialColor.rgb;
        }
        color.a *= _materialColor.a;

        #if _TOOLMODE
        if (0 != (DisplayFlags::InstanceColor & _flags)) 
        #endif
        {
            color.rgb *= _instanceColor.rgb;
        }
        color.a *= _instanceColor.a;

        return color;
    }

    //--------------------------------------------------------------------------------------
    float2 GetUV0(float2 _uv0, float2 _uv1, float3 _worldPos)
    {
        float2 uv;
        switch(getUVSource())
        {
            default:
            case UVSource::UV0:
            uv = _uv0;
            break;

            case UVSource::UV1:
            uv = _uv1;
            break;

            case UVSource::PlanarX:
            uv = _worldPos.yz;
            break;

            case UVSource::PlanarY:
            uv = _worldPos.xz;
            break;

            case UVSource::PlanarZ:
            uv = _worldPos.xy;
            break;
        }

        return uv * getTiling();
    }

    //--------------------------------------------------------------------------------------
    float4 sampleTexture2D(uint _handle, sampler _sampler, float2 _uv, bool _nonUniform = false)
    {
        #if _PS
        return _nonUniform? getNonUniformTexture2D(_handle).Sample(_sampler, _uv).rgba : getTexture2D(_handle).Sample(_sampler, _uv).rgba;
        #else
        return _nonUniform? getNonUniformTexture2D(_handle).SampleLevel(_sampler, _uv, 0).rgba : getTexture2D(_handle).SampleLevel(_sampler, _uv, 0).rgba;
        #endif
    }

    //--------------------------------------------------------------------------------------
    // If the texture may have different value in different threads from a wave, 
    // then '_nonUniform' shall be 'true' to avoid rendering artifacts in AMD cards.
    //--------------------------------------------------------------------------------------
    float4 getAlbedo(float2 _uv, float4 _vertexColor, DisplayFlags _flags, bool _nonUniform = false)
    {                 
        float4 albedo = sampleTexture2D(getAlbedoTextureHandle(), albedoSampler, _uv, _nonUniform);

        #if _TOOLMODE
        if (0 == (DisplayFlags::AlbedoMap & _flags))
            albedo.rgb = pow(0.5, 0.45);
        #endif

        albedo *= _vertexColor;
    
        return albedo;
    }

    //--------------------------------------------------------------------------------------
    float4 getNormal(float2 _uv, DisplayFlags _flags, bool _nonUniform = false)
    {
        float4 normal = sampleTexture2D(getNormalTextureHandle(), normalSampler, _uv, _nonUniform);

        #if _TOOLMODE
        if (0 == (DisplayFlags::NormalMap & _flags))
            normal.xyz = float3(0.5, 0.5, 1.0);
        #endif

        return float4((normal.xy*2-1) * getNormalStrength(), normal.z, normal.w);
    }

    //--------------------------------------------------------------------------------------
    float4 getPBR(float2 _uv, bool _nonUniform = false)
    {
        float4 pbr = sampleTexture2D(getPBRTextureHandle(), pbrSampler, _uv, _nonUniform);

        pbr.r = lerp(1.0f, pbr.r, getOcclusion());
        pbr.g *= getRoughness();
        pbr.b *= getMetalness();

        return pbr;
    }

    #endif // !__cplusplus
};