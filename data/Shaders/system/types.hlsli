#pragma once

#ifdef __cplusplus

//--------------------------------------------------------------------------------------
// On the C++ side the "HLSL-like" types belong to the "vg::core" namespace, which is 
// annoying when exposing HLSL structs/enums to C++.
//--------------------------------------------------------------------------------------
    
using namespace vg::core;

//--------------------------------------------------------------------------------------
// C++ uses VG_VULKAN/VG_DX12 #defines, while HLSL uses _VULKAN/_DX12
//--------------------------------------------------------------------------------------

#if VG_VULKAN && !defined(_VULKAN)
#define _VULKAN
#endif

#if VG_DX12 && !defined(_DX12)
#define _DX12
#endif

#else

//--------------------------------------------------------------------------------------
// Ignore the "inline" keyword on HLSL-side but we still want the functions to be inline 
// when compiled with C++.
//--------------------------------------------------------------------------------------

#define inline
#define constexpr

//--------------------------------------------------------------------------------------
// When compiling HLSL, process vg_enum[_class] shared declarations as regular enum [class] declarations
//--------------------------------------------------------------------------------------
#define vg_enum(Type, UnderlyingType, ...)       enum Type : UnderlyingType { __VA_ARGS__};
#define vg_enum_class(Type, UnderlyingType, ...) enum class Type : UnderlyingType { __VA_ARGS__};

#endif