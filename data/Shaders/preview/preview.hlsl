#include "system/bindless.hlsli"
#include "system/semantics.hlsli"
#include "system/samplers.hlsli"
#include "system/gamma.hlsli"
#include "system/rootConstants2D.hlsli"
#include "system/color.hlsli"
#include "system/view.hlsli" 
#include "system/msaa.hlsli"

struct VS_Output
{
    float4 pos  : Position;
    float2 uv   : Texcoord0;
};

VS_Output VS_Preview(uint _vertexID : VertexID)
{
    VS_Output output;

    output.uv = rootConstants2D.quad.getUV0(_vertexID);
    output.pos = rootConstants2D.quad.getPosition(_vertexID); 

    return output;
}

struct PS_Input
{
    linear noperspective float4 pos : Position;
    linear noperspective float2 uv  : Texcoord0;   
};

struct PS_Output
{
    float4 color0 : Color0;
};

PS_Output PS_Preview(PS_Input _input)
{
    PS_Output ps_out = (PS_Output)0;
    float2 uv = _input.uv;

    #if _TEXCUBE
    float2 delta = float2(uv.x-0.5, 1-uv.y-0.5);
    float dist = length(delta);
    
    //if (dist < 0.5)
    //{
    //    float theta = atan2(delta.y, delta.x);
    //    float phi = acos(dist / 0.5);
    //
    //    // Calculate the direction vector in view space
    //    float3 uvw;
    //    uvw.x = sin(phi) * cos(theta);
    //    uvw.y = cos(phi);
    //    uvw.z = sin(phi) * sin(theta);
    //
    //    ps_out.color0 = getTextureCube(rootConstants2D.texID).SampleLevel(linearRepeat, uvw, 0).rgba;
    //
    //}
    //else
    //{
    //    ps_out.color0 = float4(0,0,0,1);
    //}

    float2 ndc = _input.uv * 2.0f - 1.0f;
    float3 uvw;
    uint face = 0;
    switch(face)
    {
        case 0:
            uvw = float3(1.0, ndc.x, -ndc.y);
            break;
    }
    uvw = normalize(uvw);
    ps_out.color0 = getTextureCube(rootConstants2D.texID).SampleLevel(linearRepeat, uvw, 0).rgba;

    #else
    ps_out.color0 = 0;
    #endif

    ps_out.color0.rgb = Linear2sRGB( ps_out.color0.rgb);

    return ps_out;
}


