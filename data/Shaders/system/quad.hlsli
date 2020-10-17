#ifndef _QUAD__HLSLI_
#define _QUAD__HLSLI_

struct QuadConstants
{
    uint2 posOffset;
    uint2 posScale;
    uint2 uvOffset;
    uint2 uvScale;

    float2 getUV0(uint _vertexID)
    {
        float2 tmp = float2(_vertexID & 1, (_vertexID & 2) >> 1);
        return tmp * asfloat(uvScale) + asfloat(uvOffset);
    }

    float4 getPosition(uint _vertexID)
    {
        float2 tmp = float2(_vertexID & 1, (_vertexID & 2) >> 1);
        float2 pos = tmp * asfloat(posScale) + asfloat(posOffset);
               pos = float2(pos.x, 1 - pos.y);
        
        return float4(pos * 2 - 1, 0, 1);
    }
};

#endif // _QUAD__HLSLI_