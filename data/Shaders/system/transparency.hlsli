#ifndef _TRANSPARENCY__HLSLI_
#define _TRANSPARENCY__HLSLI_

#include "system/samplers.hlsli"
#include "system/transparentPass.hlsli"

//--------------------------------------------------------------------------------------
void applyDepthTransparency(inout float _alpha, float2 _screenPos, float4 _vpos, float _invDepthFade)
{
    TransparentPassConstants transparentPassConstants;
    transparentPassConstants.Load(getBuffer(RESERVEDSLOT_BUFSRV_TRANSPARENTPASS));

    float linearDepthBuffer = getTexture2D(transparentPassConstants.getLinearDepth()).SampleLevel(nearestClamp, _screenPos.xy, 0).y;
    float linearZ = -_vpos.z; 
    float alpha = saturate((linearDepthBuffer - linearZ) * _invDepthFade ); 
    _alpha *= alpha;
}

//--------------------------------------------------------------------------------------
bool linearDepthTest(float2 _screenPos, float4 _vpos)
{
    TransparentPassConstants transparentPassConstants;
    transparentPassConstants.Load(getBuffer(RESERVEDSLOT_BUFSRV_TRANSPARENTPASS));

    float linearDepthBuffer = getTexture2D(transparentPassConstants.getLinearDepth()).SampleLevel(nearestClamp, _screenPos.xy, 0).y;
    float linearZ = -_vpos.z; 
    return linearZ < linearDepthBuffer + 0.01;
}

//--------------------------------------------------------------------------------------
bool linearDepthTestEx(float2 _screenPos, float4 _vpos, float _fadeBias, float _fadeDist, inout float _fade)
{
    TransparentPassConstants transparentPassConstants;
    transparentPassConstants.Load(getBuffer(RESERVEDSLOT_BUFSRV_TRANSPARENTPASS));

    float linearDepthBuffer = getTexture2D(transparentPassConstants.getLinearDepth()).SampleLevel(nearestClamp, _screenPos.xy, 0).y;
    float linearZ = -_vpos.z; 
    
    float dist = linearDepthBuffer - linearZ;
    _fade = -(dist + _fadeBias) / _fadeDist;

    return 0 < dist + 0.01;
}

#endif // _TRANSPARENCY__HLSLI_