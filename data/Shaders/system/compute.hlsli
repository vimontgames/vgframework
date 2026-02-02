#ifndef _COMPUTE__HLSLI_
#define _COMPUTE__HLSLI_

float2 GetScreenUVFromCoords(int2 _coord, uint2 _size, bool _halfTexelOffset = true)
{
    float offset = _halfTexelOffset ? 0.5f : 0.0f;    
    return float2((float)(_coord.x+offset) / (float)(_size.x), (float)(_coord.y+offset) / (float)(_size.y));
}

#endif
