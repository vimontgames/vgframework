#pragma once

#include "system/packing.hlsli"
#include "system/table.hlsli"
#include "system/packing.hlsli"
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

#if 0
#define emissiveSampler anisotropicRepeat 
#else
#define emissiveSampler linearRepeat 
#endif

struct GPUMaterialData
{
    #ifdef __cplusplus
    GPUMaterialData()
    {
        textures     = (uint4)0;    
        albedoColor  = (float4)0.0;
        multipliers  = (float4)0.0;
        tilingOffset = (float4)0.0;
        misc         = (float4)0.0;

        setAlbedoTextureHandle(RESERVEDSLOT_TEXSRV_DEFAULT_ALBEDO);
        setNormalTextureHandle(RESERVEDSLOT_TEXSRV_DEFAULT_NORMAL);
        setPBRTextureHandle(RESERVEDSLOT_TEXSRV_DEFAULT_PBR);
        setEmissiveTextureHandle(RESERVEDSLOT_TEXSRV_DEFAULT_EMISSIVE);
        
        setAlbedoColor(float4(1,1,1,1));
        setNormalStrength(1.0);
        setOcclusion(1.0);
        setRoughness(0.25);
        setMetalness(0.0);
        setTiling(float2(1.0, 1.0));
        setOffset(float2(0.0, 0.0));
        setEmissiveColor(float4(0,0,0,0));
    }   
    #endif 

    uint4  textures;        // .x = normal<<16 | albedo, .y = emissive<<16 | pbr, .z = unused, .w = { SurfaceType(31..28) |  UVSource (27..24) | unused (23..0) }
    float4 albedoColor;     // TODO: pack as RGBE?
    float4 multipliers;
    float4 tilingOffset;
    float4 misc;            // .x = depth fade (used only with alphablend) | .y = emissive color (RGBA32) | .z = emissive intensity | w = unused 

    void            setAlbedoTextureHandle  (uint _value)           { textures.x = packUint16low(textures.x, _value); }
    uint            getAlbedoTextureHandle  ()                      { return unpackUint16low(textures.x); }

    void            setNormalTextureHandle  (uint _value)           { textures.x = packUint16high(textures.x, _value); }
    uint            getNormalTextureHandle  ()                      { return unpackUint16high(textures.x); }

    void            setPBRTextureHandle     (uint _value)           { textures.y = packUint16low(textures.y, _value); }
    uint            getPBRTextureHandle     ()                      { return unpackUint16low(textures.y); }
    
    void            setEmissiveTextureHandle(uint _value)           { textures.y = packUint16high(textures.y, _value); }
    uint            getEmissiveTextureHandle()                      { return unpackUint16high(textures.y); }
    
    void            setUVSource             (UVSource _value)       { textures.w = packUint(textures.w, (uint)_value, 0xF, 24); }    // Use bits 24..27 
    UVSource        getUVSource             ()                      { return (UVSource)unpackUint(textures.w, 0xF, 24); }            // As crazy as it seems, using 0x7 here instead of 0xF provokes lost device in Vulkan debug
    
    void            setSurfaceType          (SurfaceType _value)    { textures.w = packUint(textures.w, (uint)_value, 0xF, 28); }    // Use bits 28..31
    SurfaceType     getSurfaceType          ()                      { return (SurfaceType)unpackUint(textures.w, 0xF, 28); }

    void            setAlbedoColor          (float4 _value)         { albedoColor = _value; }
    float4          getAlbedoColor          ()                      { return albedoColor; }
    
    void            setNormalStrength       (float _value)          { multipliers.w = _value; }
    float           getNormalStrength       ()                      { return multipliers.w; }
    
    void            setOcclusion            (float _value)          { multipliers.x = _value; }
    float           getOcclusion            ()                      { return multipliers.x; }
    
    void            setRoughness            (float _value)          { multipliers.y = _value; }
    float           getRoughness            ()                      { return multipliers.y; }
    
    void            setMetalness            (float _value)          { multipliers.z = _value; }
    float           getMetalness            ()                      { return multipliers.z; }

    void            setTiling               (float2 _value)         { tilingOffset.xy = _value; }
    float2          getTiling               ()                      { return tilingOffset.xy; }

    void            setOffset               (float2 _value)         { tilingOffset.zw = _value; }
    float2          getOffset               ()                      { return tilingOffset.zw; }

    void            setDepthFade            (float _value)          { misc.x = 1.0f / _value; }
    float           getInvDepthFade         ()                      { return misc.x; }
    
    void            setEmissiveColor        (float4 _value)         { misc.y = asfloat(packRGBA8(_value)); }
    float4          getEmissiveColor        ()                      { return unpackRGBA8(asuint((float)misc.y)); }
    
    void            setEmissiveIntensity    (float _value)          { misc.z = _value; }
    float           getEmissiveIntensity    ()                      { return misc.z; }

    #ifndef __cplusplus
    //--------------------------------------------------------------------------------------
    float4 getVertexColorOut(float4 _vertexColor, float4 _instanceColor, DisplayFlags _flags, DisplayMode _mode)
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

        #if _TOOLMODE
        if (DisplayMode::Instance_Color == _mode)
            color.rgba = _instanceColor.rgba;
        #endif

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
            uv = float2(_worldPos.x, -_worldPos.z);
            break;
            
            case UVSource::PlanarZ:
            uv = _worldPos.xy;
            break;
        }

        return uv * getTiling() + getOffset();
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
    float4 getAlbedo(float2 _uv, float4 _vertexColor, DisplayFlags _flags, DisplayMode _mode, bool _nonUniform = false)
    {                 
        float4 albedo = sampleTexture2D(getAlbedoTextureHandle(), albedoSampler, _uv, _nonUniform);

        #if _TOOLMODE
        if (0 == (DisplayFlags::AlbedoMap & _flags))
            albedo.rgb = pow(0.5, 0.45);
        #endif

        albedo *= _vertexColor;

        #if _TOOLMODE
        if (DisplayMode::Instance_Color == _mode)
            albedo = _vertexColor;
        #endif
    
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
    
    //--------------------------------------------------------------------------------------
    float4 getEmissive(float2 _uv, bool _nonUniform = false)
    {
        float4 emissive = sampleTexture2D(getEmissiveTextureHandle(), emissiveSampler, _uv, _nonUniform);

        emissive.rgb *= getEmissiveColor().rgb * getEmissiveIntensity();

        return emissive;
    }

    #endif // !__cplusplus
};