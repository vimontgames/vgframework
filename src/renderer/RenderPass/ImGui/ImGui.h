#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS

#pragma push_macro("new")
#undef new
#include "../extern/imgui/imgui.h"
#include "imgui/imgui_internal.h"
#pragma pop_macro("new")

//--------------------------------------------------------------------------------------
inline ImVec2 float2ToImVec2(const vg::core::float2 & _f2)
{
    return ImVec2(_f2.x, _f2.y);
}

//--------------------------------------------------------------------------------------
inline vg::core::float2 ImVec2ToFloat2(const ImVec2 & _vec2)
{
    return vg::core::float2(_vec2.x, _vec2.y);
}

//--------------------------------------------------------------------------------------
inline ImVec4 float4ToImVec4(const vg::core::float4 & _f4)
{
    return ImVec4(_f4.x, _f4.y, _f4.z, _f4.w);
}

//--------------------------------------------------------------------------------------
inline vg::core::float4 ImVec4ToFloat4(const ImVec4 & _vec4)
{
    return vg::core::float4(_vec4.x, _vec4.y, _vec4.z, _vec4.w);
}