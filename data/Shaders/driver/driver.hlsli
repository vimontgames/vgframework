#ifndef _DRIVER__HLSLI_
#define _DRIVER__HLSLI_

#ifdef DX12
#define DECL_ROOTCONSTANTS(type, name, reg, s) ConstantBuffer<type> name : register(b##reg, space##s);
#elif defined(VULKAN)
#define DECL_ROOTCONSTANTS(type, name, reg, s) [[vk::push_constant]] ConstantBuffer<type> name : register(b##reg, space##s);
#endif

struct RootConstants
{
    uint2 posOffset;
    uint2 posScale;
    uint2 uvOffset;
    uint2 uvScale;
    uint  texID;
};

#ifndef __cplusplus
DECL_ROOTCONSTANTS(RootConstants, rootConstants, 0, 0);
#endif

#endif // _DRIVER__HLSLI_