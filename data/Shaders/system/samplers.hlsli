#ifndef _SAMPLERS__HLSLI_
#define _SAMPLERS__HLSLI_
#ifndef __cplusplus

//--------------------------------------------------------------------------------------
// Should match enum in "SamplerState_consts.h"
//--------------------------------------------------------------------------------------

#ifdef VG_DX12

sampler nearestClamp  : register(s0, space0);
sampler nearestRepeat : register(s1, space0);
sampler linearClamp   : register(s2, space0);
sampler linearRepeat  : register(s3, space0);

SamplerComparisonState shadowcmp     : register(s4, space0);

#elif defined(VG_VULKAN)

[[vk::binding(0, 1)]] sampler vkSamplerArray[5]  : register(s0, space1);
#define nearestClamp  vkSamplerArray[0]
#define nearestRepeat vkSamplerArray[1]
#define linearClamp   vkSamplerArray[2]
#define linearRepeat  vkSamplerArray[3]

[[vk::binding(0, 1)]] SamplerComparisonState vkSamplerComparisonArray[5]  : register(s0, space1);
#define shadowcmp     vkSamplerComparisonArray[4]

#endif

#endif // __cplusplus
#endif // _SAMPLERS__HLSLI_