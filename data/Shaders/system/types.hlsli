#pragma once

#ifdef __cplusplus

//--------------------------------------------------------------------------------------
// On the C++ side the "HLSL-like" types belong to the "vg::core" namespace, which is 
// annoying when exposing HLSL structs/enums to C++.
//--------------------------------------------------------------------------------------
    
using namespace vg::core;

#else

//--------------------------------------------------------------------------------------
// Ignore the "inline" keyword on HLSL-side but we still want the functions to be inline 
// when compiled with C++.
//--------------------------------------------------------------------------------------

#define inline

#endif