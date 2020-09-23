#ifndef _DRIVER__HLSLI_
#define _DRIVER__HLSLI_

#include "semantics.hlsli"

#ifdef DX12
#define DECL_ROOTCONSTANTS(type, name, reg, s) ConstantBuffer<type> name : register(b##reg, space##s);
#elif defined(VULKAN)
#define DECL_ROOTCONSTANTS(type, name, reg, s) [[vk::push_constant]] ConstantBuffer<type> name : register(b##reg, space##s);
#endif

#ifndef __cplusplus
struct RootConstants
{
    uint4 posOffsetScale;
    uint4 uvOffsetScale;
};
DECL_ROOTCONSTANTS(RootConstants, rootConstants, 0, 0);

Texture2D Texture2DTable[65535] : register(t0, space0);

SamplerState        sampler0        : register(s0);


#endif

#endif // _DRIVER__HLSLI_