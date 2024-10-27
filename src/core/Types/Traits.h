#pragma once

#include "Types.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // Vector type traits for metaprogramming usage
    //--------------------------------------------------------------------------------------

    template <typename T> struct vectorTraits { using type = T;    static const uint count = 1; static T   makeVector(const T * _values) { return *_values; } };

    template <> struct vectorTraits<u8>     { using type = u8;    static const uint count = 1; static u8     makeVector(const u8 * _values) { return *_values; } };
    template <> struct vectorTraits<u16>    { using type = u16;   static const uint count = 1; static u16    makeVector(u16 * _values)      { return _values[0]; } };
    template <> struct vectorTraits<u32>    { using type = u32;   static const uint count = 1; static u32    makeVector(u32 * _values)      { return _values[0]; } };
    template <> struct vectorTraits<u64>    { using type = u64;   static const uint count = 1; static u64    makeVector(u64 * _values)      { return _values[0]; } };
                                                                  
    template <> struct vectorTraits<uint2>  { using type = u32;   static const uint count = 2; static uint2  makeVector(u32 * _values)      { return uint2(_values[0], _values[1]); } };
    template <> struct vectorTraits<uint3>  { using type = u32;   static const uint count = 3; static uint3  makeVector(u32 * _values)      { return uint3(_values[0], _values[1], _values[2]); } };
    template <> struct vectorTraits<uint4>  { using type = u32;   static const uint count = 4; static uint4  makeVector(u32 * _values)      { return uint4(_values[0], _values[1], _values[2], _values[3]); } };
                                                                  
    template <> struct vectorTraits<i8>     { using type = i8;    static const uint count = 1; static i8     makeVector(i8 * _values)       { return _values[0]; } };
    template <> struct vectorTraits<i16>    { using type = i16;   static const uint count = 1; static i16    makeVector(i16 * _values)      { return _values[0]; } };
    template <> struct vectorTraits<i32>    { using type = i32;   static const uint count = 1; static i32    makeVector(i32 * _values)      { return _values[0]; } };
    template <> struct vectorTraits<i64>    { using type = i64;   static const uint count = 1; static i64    makeVector(i64 * _values)      { return _values[0]; } };
                                                                  
    template <> struct vectorTraits<int2>   { using type = i32;   static const uint count = 2; static int2   makeVector(i32 * _values)      { return int2(_values[0], _values[1]); } };
    template <> struct vectorTraits<int3>   { using type = i32;   static const uint count = 3; static int3   makeVector(i32 * _values)      { return int3(_values[0], _values[1], _values[2]); } };
    template <> struct vectorTraits<int4>   { using type = i32;   static const uint count = 4; static int4   makeVector(i32 * _values)      { return int4(_values[0], _values[1], _values[2], _values[3]); } };

    template <> struct vectorTraits<float>  { using type = float; static const uint count = 1; static float  makeVector(float * _values)    { return _values[0]; } };
    template <> struct vectorTraits<float2> { using type = float; static const uint count = 2; static float2 makeVector(float * _values)    { return float2(_values[0], _values[1]); } };
    template <> struct vectorTraits<float3> { using type = float; static const uint count = 3; static float3 makeVector(float * _values)    { return float3(_values[0], _values[1], _values[2]); } };
    template <> struct vectorTraits<float4> { using type = float; static const uint count = 4; static float4 makeVector(float * _values)    { return float4(_values[0], _values[1], _values[2], _values[3]); } };

    template <> struct vectorTraits<float4x4> { using type = float; static const uint count = 16; static float4x4 makeVector(float * _values) { return float4x4(_values[0], _values[1], _values[2], _values[3], _values[4], _values[5], _values[6], _values[7], _values[8], _values[9], _values[10], _values[11], _values[12], _values[13], _values[14], _values[15] ); } };

    template <typename T> struct scalarTraits;
    template <> struct scalarTraits<u8>     { using larger_type = u64; using signed_type = i8;  using unsigned_type = u8;  static const bool is_signed = false; static const bool is_integer = true; };
    template <> struct scalarTraits<u16>    { using larger_type = u64; using signed_type = i16; using unsigned_type = u16; static const bool is_signed = false; static const bool is_integer = true; };
    template <> struct scalarTraits<u32>    { using larger_type = u64; using signed_type = i32; using unsigned_type = u32; static const bool is_signed = false; static const bool is_integer = true; };
    template <> struct scalarTraits<u64>    { using larger_type = u64; using signed_type = i64; using unsigned_type = u64; static const bool is_signed = false; static const bool is_integer = true; };
                                                                                                      
    template <> struct scalarTraits<i8>     { using larger_type = i64; using signed_type = i8;  using unsigned_type = u8;  static const bool is_signed = true;  static const bool is_integer = true; };
    template <> struct scalarTraits<i16>    { using larger_type = i64; using signed_type = i16; using unsigned_type = u16; static const bool is_signed = true;  static const bool is_integer = true; };
    template <> struct scalarTraits<i32>    { using larger_type = i64; using signed_type = i32; using unsigned_type = u32; static const bool is_signed = true;  static const bool is_integer = true; };
    template <> struct scalarTraits<i64>    { using larger_type = i64; using signed_type = i64; using unsigned_type = u64; static const bool is_signed = true;  static const bool is_integer = true; };

    template <> struct scalarTraits<float>  { static const bool is_signed = true;  static const bool is_integer = false; };
    template <> struct scalarTraits<double> { static const bool is_signed = true;  static const bool is_integer = false; };

    template <> struct scalarTraits<bool>   { static const bool is_signed = false;  static const bool is_integer = false; };
}