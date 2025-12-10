#ifndef _GBUFFER__HLSLI_
#define _GBUFFER__HLSLI_

//--------------------------------------------------------------------------------------
// Read from GBuffer
//--------------------------------------------------------------------------------------
struct GBufferSample
{
    float3 albedo;
    float3 normal;
    float3 pbr; 
    float3 emissive;

    void Load(int2 _coords, uint _albedoGBuffer, uint _normalGBuffer, uint _pbrGBuffer, uint _emissiveGBuffer, uint _sampleIndex = 0)
    {
        #if SAMPLE_COUNT > 1
        albedo   = getTexture2DMS(  _albedoGBuffer).Load(_coords, _sampleIndex).rgb;
        normal   = getTexture2DMS(  _normalGBuffer).Load(_coords, _sampleIndex).xyz * 2.0f - 1.0f;
        pbr      = getTexture2DMS(     _pbrGBuffer).Load(_coords, _sampleIndex).rgb; 
        emissive = getTexture2DMS(_emissiveGBuffer).Load(_coords, _sampleIndex).rgb; 
        #else
        albedo   = getTexture2D(  _albedoGBuffer).Load(int3(_coords,0)).rgb;
        normal   = getTexture2D(  _normalGBuffer).Load(int3(_coords,0)).xyz * 2.0f - 1.0f;
        pbr      = getTexture2D(     _pbrGBuffer).Load(int3(_coords,0)).rgb;    
        emissive = getTexture2D(_emissiveGBuffer).Load(int3(_coords,0)).rgb;  
        #endif
    }
};

//--------------------------------------------------------------------------------------
// Write to GBuffer
// Alpha value is only used for alpha blending (e.g. when rendering decals)
//--------------------------------------------------------------------------------------
struct PS_GBufferOutput
{
    float4 albedo   : Color0;
    float4 normal   : Color1;
    float4 pbr      : Color2;
    float4 emissive : Color3;
    
    void Store(float3 _albedo, float3 _normal, float3 _pbr, float3 _emissive, float _alpha = 1.0f)
    {
        albedo   = float4(  _albedo.rgb, _alpha);
        normal   = float4(  _normal.xyz * 0.5f + 0.5f, _alpha);
        pbr      = float4(     _pbr.rgb, _alpha);
        emissive = float4(_emissive.rgb, _alpha);    
    }
};

#endif // _GBUFFER__HLSLI_