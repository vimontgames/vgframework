#pragma once 

//--------------------------------------------------------------------------------------
// Platform
//--------------------------------------------------------------------------------------
#ifdef _WIN32
#define VG_WINDOWS 1
#endif

//--------------------------------------------------------------------------------------
// Configuration
//--------------------------------------------------------------------------------------
#ifdef _DEBUG
#define VG_DEBUG
#else
#define VG_RELEASE
#endif

//--------------------------------------------------------------------------------------
// Platform includes
//--------------------------------------------------------------------------------------
#ifdef VG_WINDOWS
#define NOMINMAX
#include <windows.h>
#include <system_error>
#endif

//--------------------------------------------------------------------------------------
// Debug macros
//--------------------------------------------------------------------------------------
#ifdef VG_DEBUG
	#define VG_ENABLE_ASSERT		1
	#define VG_ENABLE_DEBUGPRINT	1
#elif defined(VG_RELEASE)
	//#define VG_ENABLE_ASSERT		1
	//#define VG_ENABLE_DEBUGPRINT	1
#endif

#if VG_ENABLE_DEBUGPRINT
#define VG_DEBUGPRINT(...) vg::core::debugPrint(__VA_ARGS__)
#else
#define VG_DEBUGPRINT(...) {}
#endif

#if VG_ENABLE_ASSERT
#define VG_ASSERT(condition, ...)															        \
	do																						        \
	{																						        \
		if (!(condition))																	        \
		{																					        \
			static bool skip = false;														        \
			if (vg::core::assertmsg(#condition, __func__, __FILE__, __LINE__, skip, ##__VA_ARGS__))	\
				__debugbreak();																        \
		}																					        \
	}																						        \
	while(0)
#define VG_VERIFY(f) VG_ASSERT(f)
#else
#define VG_ASSERT(condition, ...)	{}
#define VG_VERIFY(f) f
#endif

#define VG_ASSERT_NOT_IMPLEMENTED() VG_ASSERT(false, "Function %s is not implemented", __func__)

#define VG_SAFE_FREE(p)	 { if (p) { free(p); p = nullptr;} }
#define VG_SAFE_DELETE(p)  { if (p) { delete p; p = nullptr;} }
#define VG_SAFE_RELEASE(p) { if (p) { p->Release(); p = nullptr; } }

//--------------------------------------------------------------------------------------
// standard includes
//--------------------------------------------------------------------------------------
#include <string>
#include <atomic>
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>

//--------------------------------------------------------------------------------------
// external includes
//--------------------------------------------------------------------------------------
#include "hlslpp/include/hlsl++.h"
#include "magic_enum/include/magic_enum.hpp"

//--------------------------------------------------------------------------------------
// core
//--------------------------------------------------------------------------------------
#include "core.h"

namespace vg
{
	namespace core
	{
		template <typename T, std::size_t N>
		constexpr std::size_t countof(T const (&)[N]) noexcept
		{
			return N;
		}

		template <typename T> constexpr uint64_t countBits(T _value)
		{
			return (uint64_t)log2(uint64_t(1) << ((sizeof(_value) << 3) - 1)) + 1;
		}

		template <typename E> constexpr auto enumCount()		{ return magic_enum::enum_count<E>(); }
		template <typename E> constexpr auto enumValues()		{ return magic_enum::enum_values<E>(); }
		template <typename E> constexpr auto isEnumValue(E e)	{ return magic_enum::enum_contains(e); }
		
		template <typename E> constexpr auto operator&(E lhs, E rhs)	{ return static_cast<E>(static_cast<magic_enum::underlying_type_t<E>>(lhs) & static_cast<magic_enum::underlying_type_t<E>>(rhs)); }
		template <typename E> constexpr auto operator|(E lhs, E rhs)	{ return static_cast<E>(static_cast<magic_enum::underlying_type_t<E>>(lhs) | static_cast<magic_enum::underlying_type_t<E>>(rhs)); }
		template <typename E> constexpr auto operator|=(E& lhs, E rhs)	{ return lhs = lhs | rhs; }

		template <typename E> constexpr auto asString(E e)		{ auto name = (std::string)magic_enum::enum_name(e); VG_ASSERT(!name.empty(), "Could not retrieve %s enum name for value '%u'", ((std::string)(magic_enum::enum_type_name<E>())).c_str(), magic_enum::enum_integer(e)); return name; }
		template <typename E> constexpr auto asInteger(E e)		{ return magic_enum::enum_integer(e); }
		template <typename E> constexpr bool asBool(E _e)		{ return 0 != magic_enum::enum_integer(_e); }

		using WinHandle = void*;
		static const WinHandle invalidWindowHandle = (WinHandle)-1;

		using InstanceHandle = void*;
		static const InstanceHandle invalidInstanceHandle = (InstanceHandle)-1;
	}
}