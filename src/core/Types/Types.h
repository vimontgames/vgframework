#pragma once

namespace vg::core
{
	using string	= std::string;
    using wstring   = std::wstring;

	using i8		=  int8_t;
	using u8		=  uint8_t;
	using i16		=  int16_t;
	using u16		=  uint16_t;
	using i32		=  int32_t;
	using u32		=  uint32_t;
	using i64		=  int64_t;
	using u64		=  uint64_t;

	using uint		= unsigned int;

	using uint_ptr	= u64;
	using int_ptr	= i64;

	using float1	= hlslpp::float1;
	using float2	= hlslpp::float2;
	using float3	= hlslpp::float3;
	using float4	= hlslpp::float4;

	using int1		= hlslpp::int1;
	using int2		= hlslpp::int2;
	using int3		= hlslpp::int3;
	using int4		= hlslpp::int4;

	using uint1		= hlslpp::uint1;
	using uint2		= hlslpp::uint2;
	using uint3		= hlslpp::uint3;
	using uint4		= hlslpp::uint4;

	using double1	= hlslpp::double1;
	using double2	= hlslpp::double2;
	using double3	= hlslpp::double3;
	using double4	= hlslpp::double4;

	using float1x1	= hlslpp::float1x1;
	using float1x2	= hlslpp::float1x2;
	using float1x3	= hlslpp::float1x3;
	using float1x4	= hlslpp::float1x4;

	using float2x1	= hlslpp::float2x1;
	using float2x2	= hlslpp::float2x2;
	using float2x3	= hlslpp::float2x3;
	using float2x4	= hlslpp::float2x4;

	using float3x1	= hlslpp::float3x1;
	using float3x2	= hlslpp::float3x2;
	using float3x3	= hlslpp::float3x3;
	using float3x4	= hlslpp::float3x4;

	using float4x1	= hlslpp::float4x1;
	using float4x2	= hlslpp::float4x2;
	using float4x3	= hlslpp::float4x3;
	using float4x4	= hlslpp::float4x4;
    
    using size_t = std::size_t;

    template <typename T> using atomic = std::atomic<T>;

    using mutex = std::mutex;
    template <typename T> using lock_guard = std::lock_guard<T>;

	template <typename T> using vector = std::vector<T>;
    template <typename T1, typename T2> using pair = std::pair<T1, T2>;
    
    template <typename T> using hash = std::hash<T>;
	template <typename K, typename V> using map				            = std::map<K, V>;
	template <typename K, typename V, typename H> using unordered_map	= std::unordered_map<K, V, H>;

	template <typename T> using set							            = std::set<T>;
	template <typename T> using unordered_set				            = std::unordered_set<T>;

    template <typename T> const auto to_string(T _val)
    {
        return std::to_string(_val);
    }

    template <typename T> using dictionary = core::unordered_map<core::string, T, core::hash<core::string> >;
}