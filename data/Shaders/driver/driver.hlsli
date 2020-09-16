#ifndef _DRIVER__HLSLI_
#define _DRIVER__HLSLI_

#include "semantics.hlsli"

#ifndef __cplusplus
struct RootConstants
{
    uint4 posOffsetScale;
    uint4 uvOffsetScale;
};
[[vk::push_constant]] ConstantBuffer<RootConstants> rootConstants : register(b0, space0);

#endif

#endif // _DRIVER__HLSLI_