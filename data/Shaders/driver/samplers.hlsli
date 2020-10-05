#ifndef _SAMPLERS__HLSLI_
#define _SAMPLERS__HLSLI_
#ifndef __cplusplus

//--------------------------------------------------------------------------------------
// Should match enum in "SamplerState_consts.h"
//--------------------------------------------------------------------------------------

#ifdef DX12
sampler nearestClamp  : register(s0, space0);
sampler nearestRepeat : register(s1, space0);
sampler linearClamp   : register(s2, space0);
sampler linearRepeat  : register(s3, space0);
#elif defined(VULKAN)
[[vk::binding(0, 1)]] sampler vkSamplerArray[4]  : register(s0, space1);
#define nearestClamp  vkSamplerArray[0]
#define nearestRepeat vkSamplerArray[1]
#define linearClamp   vkSamplerArray[2]
#define linearRepeat  vkSamplerArray[3]
#endif

#endif // __cplusplus
#endif // _SAMPLERS__HLSLI_