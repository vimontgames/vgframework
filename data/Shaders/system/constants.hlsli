#ifndef _CONSTANTS__HLSLI_
#define _CONSTANTS__HLSLI_

#ifdef VG_DX12
#define DECL_ROOTCONSTANTS(type, name, reg, s) ConstantBuffer<type> name : register(b##reg, space##s)
#elif defined(VG_VULKAN)
#define DECL_ROOTCONSTANTS(type, name, reg, s) [[vk::push_constant]] type name : register(b##reg, space##s)
#endif  

#endif // _CONSTANTS__HLSLI_