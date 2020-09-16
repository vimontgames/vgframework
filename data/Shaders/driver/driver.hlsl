#include "driver.hlsli"

struct VS_Output_Quad
{
    float4 pos  : Position;
    float2 uv   : Texcoord0;
};

VS_Output_Quad VS_Quad(uint _vertexID : VertexID)
{
    VS_Output_Quad output;

    float2 tmp = float2(_vertexID & 1, (_vertexID & 2) >> 1);
    float2 uv = tmp * asfloat(rootConstants.uvOffsetScale.zw) + asfloat(rootConstants.uvOffsetScale.xy);
    output.uv = uv;
    
    float2 pos = tmp * asfloat(rootConstants.posOffsetScale.zw) + asfloat(rootConstants.posOffsetScale.xy);
           pos = float2(pos.x, 1 - pos.y);
    output.pos = float4(pos * 2 - 1, 0, 1);

    return output;
}

struct PS_Output_Quad
{
    float4 color0 : Color0;
};

PS_Output_Quad PS_Quad(VS_Output_Quad _input)
{
    PS_Output_Quad output;
    output.color0 = float4(frac(_input.uv), 0, 1);

    //#ifdef VULKAN
    //output.color0.b = 1;
    //#endif
    //
    //#ifdef DX12
    //output.color0.b = 0.5;
    //#endif

    return output;
}



