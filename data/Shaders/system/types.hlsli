#ifndef _TYPES_HLSLI_
#define _TYPES_HLSLI_

#ifdef __cplusplus

//--------------------------------------------------------------------------------------
// C++
//
//--------------------------------------------------------------------------------------

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

using u8  = vg::core::u8;
using u16 = vg::core::u16;
using u32 = vg::core::u32;

inline bool IsToolMode()
{
    VG_ASSERT(false, "This function should only be called from shader code");
    return false;
}

#else

//--------------------------------------------------------------------------------------
// HLSL
//
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Ignore the "inline" keyword on HLSL-side but we still want the functions to be inline 
// when compiled with C++.
//--------------------------------------------------------------------------------------

#define inline
#define constexpr

//--------------------------------------------------------------------------------------
// When compiling HLSL, process vg_enum[_class] shared declarations as regular enum [class] declarations
//--------------------------------------------------------------------------------------
#define vg_enum_global(Type, UnderlyingType, ...)       enum Type : uint { __VA_ARGS__};
#define vg_enum_class_global(Type, UnderlyingType, ...) enum class Type : uint { __VA_ARGS__};

//--------------------------------------------------------------------------------------
// Workaround for Visual Studio HLSL syntax highlighting. Use to declare macros values for easier source code reading.
//--------------------------------------------------------------------------------------
#if defined(_VULKAN) || defined(_DX12)
#define _SHADER_COMPILER 1
#endif

//--------------------------------------------------------------------------------------
// Helper macro to use instead of #ifdef
//--------------------------------------------------------------------------------------
inline bool IsToolMode()
{
    #if _TOOLMODE
    return true;
    #else
    return false;
    #endif
}

//--------------------------------------------------------------------------------------
// Use with (enum) class
// Due to a bug with DXC only with Vuklan, can't use the template version here
//--------------------------------------------------------------------------------------

#if 0

template<typename E> bool HasAnyFlag(E _value, E _flags)
{
    return 0 != ((uint)_value & (uint)_flags);
}

template<typename E> bool HasAllFlags(E _value, E _flags)
{
    return ((uint)_flags) != ((uint)_value & (uint)_flags);
}

#else

#define HasAnyFlag(_value, _flags) (0 != ((uint)_value & (uint)_flags))

#define HasAllFlags(_value, _flags)  ((uint)_flags) != ((uint)_value & (uint)_flags)

#endif

//--------------------------------------------------------------------------------------
// HLSL does only accept int/uint enum class types but we might want smaller types in engine
//--------------------------------------------------------------------------------------
using u8  = uint;
using u16 = uint;
using u32 = uint;

#endif

#endif // _TYPES_HLSLI_