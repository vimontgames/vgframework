#ifndef _SAMPLERS__HLSLI_
#define _SAMPLERS__HLSLI_
#ifndef __cplusplus

#ifdef DX12
#define space_samplers space0
#elif defined(VULKAN)
#define space_samplers space1
#endif

sampler sampler0 : register(s0, space_samplers);

#endif // __cplusplus
#endif // _SAMPLERS__HLSLI_