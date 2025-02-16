#pragma once 

//--------------------------------------------------------------------------------------
// Platform
//--------------------------------------------------------------------------------------
#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#define _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define VG_WINDOWS 1
#endif

//--------------------------------------------------------------------------------------
// Configuration
//--------------------------------------------------------------------------------------
#if !defined(VG_DEBUG) && !defined(VG_DEVELOPMENT) && !defined(VG_FINAL)
#ifndef VG_RELEASE
#define VG_RELEASE
#endif
#endif

//--------------------------------------------------------------------------------------
// Platform includes
//--------------------------------------------------------------------------------------
#ifdef VG_WINDOWS
#define NOMINMAX

#ifdef VG_DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <winsock2.h> // must be included before <windows.h>
#include <windows.h>
#include <system_error>
#endif

//--------------------------------------------------------------------------------------
// Compiler
// When compiling using Clang under Visual studio then both _MSC_VER and __clang__ are defined
// because it means we are using Clang with the MSVC tool chain.
//--------------------------------------------------------------------------------------
#ifdef _MSC_VER
#define VG_MSVC 1
#endif

#ifdef __clang__
#define VG_CLANG 1
#endif

#if VG_CLANG
#define VG_OFFSETOF(class, member) (reinterpret_cast<std::ptrdiff_t>(&reinterpret_cast<class *>(0)->member)) // cannot always use offsetof with clang (warning : offset of on non-standard-layout type)
#else
#define VG_OFFSETOF(class, member) (offsetof(class, member))
#endif

//--------------------------------------------------------------------------------------
// Debug macros
//--------------------------------------------------------------------------------------
#ifdef VG_DEBUG
	#define VG_ENABLE_ASSERT            1
	#define VG_ENABLE_DEBUGPRINT        1
	#define VG_ENABLE_INLINE            0
	#define VG_ENABLE_GLOBAL_NEW_DELETE 1
	#define VG_ENABLE_CHECK_STATIC_CAST 1 
#elif defined(VG_DEVELOPMENT)
	#define VG_ENABLE_ASSERT            1
	#define VG_ENABLE_DEBUGPRINT        1
	#define VG_ENABLE_INLINE			0
	#define VG_ENABLE_GLOBAL_NEW_DELETE	0
	#define VG_ENABLE_CHECK_STATIC_CAST	1 
#elif defined(VG_RELEASE)
	#define VG_ENABLE_ASSERT			1
	#define VG_ENABLE_DEBUGPRINT		1
    #define VG_ENABLE_INLINE			1
	#define VG_ENABLE_GLOBAL_NEW_DELETE	0
	#define VG_ENABLE_CHECK_STATIC_CAST	0 
#elif defined(VG_FINAL)
	#define VG_ENABLE_ASSERT			0
	#define VG_ENABLE_DEBUGPRINT		0
	#define VG_ENABLE_INLINE			1
	#define VG_ENABLE_GLOBAL_NEW_DELETE	0
	#define VG_ENABLE_CHECK_STATIC_CAST	0 
#endif

//--------------------------------------------------------------------------------------
// Overrides
// e.g. use #define VG_OVERRIDE_ENABLE_GLOBAL_NEW_DELETE 1 to force VG_ENABLE_GLOBAL_NEW_DELETE to 1 
// or #define VG_OVERRIDE_ENABLE_GLOBAL_NEW_DELETE 0 to force VG_ENABLE_GLOBAL_NEW_DELETE to 0
//--------------------------------------------------------------------------------------
#ifdef VG_OVERRIDE_ENABLE_GLOBAL_NEW_DELETE 
#undef VG_ENABLE_GLOBAL_NEW_DELETE
#define VG_ENABLE_GLOBAL_NEW_DELETE VG_OVERRIDE_ENABLE_GLOBAL_NEW_DELETE
#endif

#if VG_ENABLE_GLOBAL_NEW_DELETE 

void * operator new(std::size_t _size);
void operator delete(void * ptr) noexcept;
void * operator new(std::size_t _size, const char * _file, int _line);
void operator delete(void * ptr, const char * _file, int _line) noexcept;
void * operator new(std::size_t _size, std::align_val_t _alignment, const char * _file, int _line);
void operator delete(void * ptr, std::align_val_t _alignment) noexcept;
void operator delete(void * ptr, std::align_val_t _alignment, const char * _file, int _line) noexcept;
void * operator new[](std::size_t _size, const char * _file, int _line);
void operator delete[](void * _ptr, const char * _file, int _line) noexcept;
void operator delete[](void * _ptr) noexcept;

#define VG_NEW new( __FILE__ , __LINE__ )
#define new VG_NEW
#define VG_DELETE(p) delete p

#else
#define VG_NEW new
#define VG_DELETE delete
#endif

#if VG_ENABLE_INLINE
#define VG_INLINE inline
#else
#define VG_INLINE
#endif

#if VG_ENABLE_DEBUGPRINT
#define VG_DEBUGPRINT(...) vg::core::debugPrint(__VA_ARGS__)
#else
#define VG_DEBUGPRINT(...) __noop
#endif

#ifdef _WIN32
	#include <intrin.h>
	#define VG_DEBUG_BREAK() __debugbreak()
#elif defined(__linux__) || defined(__APPLE__)
	#include <signal.h>
	#include <cstring>
	#include <cstdarg>
	#include <x86intrin.h>
	#define VG_DEBUG_BREAK() raise(SIGTRAP)
#else
	#error "Unsupported platform"
#endif

#ifdef _WIN32
	#define CALL_CONV __cdecl
#else
	#define CALL_CONV
#endif

#if VG_ENABLE_ASSERT
namespace vg::core
{
	bool assertmsg(const char * _condition, const char * _func, const char * _file, int _line, bool & _skip, const char * _format, ...);
	bool assertmsg(const char * _condition, const char * _func, const char * _file, int _line, bool & _skip);
}
#define VG_ASSERT(condition, ...)															        \
	do																						        \
	{																						        \
		if (!(condition))																	        \
		{																					        \
			static bool skip = false;														        \
			if (vg::core::assertmsg(#condition, __func__, __FILE__, __LINE__, skip, ##__VA_ARGS__))	\
				VG_DEBUG_BREAK();																        \
		}																					        \
	}																						        \
	while(0)
#define VG_VERIFY(f) VG_ASSERT(f)
#else
#define VG_ASSERT(condition, ...) __noop
#define VG_VERIFY(f) ((void)(f)) 
#endif

#define VG_ASSERT_ENUM_NOT_IMPLEMENTED(value) VG_ASSERT(false, "case %s: is not implemented", vg::core::asString(value).c_str());
#define VG_ASSERT_NOT_IMPLEMENTED() VG_ASSERT(false, "Function %s not implemented for class '%s'", __func__, GetClassName())

#define VG_STATIC_ASSERT(condition, message) static_assert(condition, message)
#define VG_STATIC_ASSERT_NOT_IMPLEMENTED() VG_STATIC_ASSERT(false, "Function is not implemented")

#define VG_STATIC_ASSERT_IS_ALIGNED(address, alignment, message) VG_STATIC_ASSERT(0 == (uint_ptr(address) & (alignment-1)), message)

#if VG_ENABLE_CHECK_STATIC_CAST
	namespace vg::core
	{
		template <typename DST, typename SRC> inline DST * SafeStaticCast(SRC * _src)
		{
			if (nullptr != _src)
			{
				VG_ASSERT(dynamic_cast<DST *>(_src));
				return (DST *)_src;
			}
			return nullptr;
		}
	};
	#define VG_SAFE_STATIC_CAST(type, ptr) vg::core::SafeStaticCast<type>(ptr)
#else
	#define VG_SAFE_STATIC_CAST(type, ptr) ((type*)ptr)
#endif

#define VG_ASSERT_IS_ALIGNED(address, alignment) VG_ASSERT(0 == (uint_ptr(address) & (alignment-1)), "%s (0x%016X) is not aligned to %u bytes", #address, address, alignment)


#define VG_SAFE_FREE(p)	 { if (p) { free((void*)(p)); (p) = nullptr;} }
#define VG_SAFE_DELETE(p)  { if (p) { delete (p); (p) = nullptr;} }
#define VG_SAFE_RELEASE(p) { if (p) { (p)->Release(); (p) = nullptr; } }

#define VG_SAFE_INCREASE_REFCOUNT(p) { if (p) { p->AddRef(); }}

#define VG_SAFE_RELEASE_ASYNC(p) { auto * factory = vg::core::Kernel::getFactory(); if (factory) factory->ReleaseAsync(p); else (p)->Release(); p = nullptr; }

#define VG_RESTRICT __restrict

#define VG_UNREFERENCED_VARIABLE(x) (void)(x)

#define VG_STRINGIFY(quoteMe) #quoteMe

//--------------------------------------------------------------------------------------
// standard includes
//--------------------------------------------------------------------------------------
#include <string>
#include <atomic>
#include <mutex>
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <functional>
#include <algorithm>

//--------------------------------------------------------------------------------------
// external includes
//--------------------------------------------------------------------------------------
#define HLSLPP_FEATURE_TRANSFORM

#include "hlslpp/include/hlsl++.h"

//--------------------------------------------------------------------------------------
// core
//--------------------------------------------------------------------------------------
#include "Core.h"

namespace vg
{
	namespace core
	{
		template <typename T, core::size_t N>
        inline constexpr core::size_t countof(T const (&)[N]) noexcept                  { return N; }

		template <typename T> inline constexpr uint64_t countBits(T _value)             { return (uint64_t)log2(uint64_t(1) << ((sizeof(_value) << 3) - 1)) + 1; }

		using WinHandle = void*;
		static const WinHandle invalidWindowHandle = (WinHandle)-1;

		using InstanceHandle = void*;
		static const InstanceHandle invalidInstanceHandle = (InstanceHandle)-1;
	}
}

//--------------------------------------------------------------------------------------
// Static reflection for enums
//--------------------------------------------------------------------------------------
#include "core/Types/Enum.h"
#include "core/Misc/GenericEnumTypes.h"
