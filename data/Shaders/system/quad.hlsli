#ifndef _QUAD__HLSLI_
#define _QUAD__HLSLI_

struct QuadConstants
{
    float4 posOffsetScale;
    float4 uvOffsetScale;

    float2 getUV0(uint _vertexID)
    {
        float2 tmp = float2(_vertexID & 1, (_vertexID & 2) >> 1);
        return tmp * (uvOffsetScale.zw) + (uvOffsetScale.xy);
    }

    float4 getPosition(uint _vertexID)
    {
        float2 tmp = float2(_vertexID & 1, (_vertexID & 2) >> 1);
        float2 pos = tmp * (posOffsetScale.zw) + (posOffsetScale.xy);
        pos = float2(pos.x, 1 - pos.y);

        return float4(pos * 2 - 1, 0, 1);
    }
};



#endif // _QUAD__HLSLI_